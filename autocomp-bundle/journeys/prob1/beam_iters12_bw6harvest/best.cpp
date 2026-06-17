#include <mu_intrinsics.h>
#include <mu_schedule.h>
#include <stdint.h>

static inline void kernel_body(
  void* raw_arg,
  uint32_t tid_in_threadblock,
  uint32_t threads_per_threadblock,
  uint32_t threadblock_id
) {
  (void)threadblock_id;
  auto* a = reinterpret_cast<KernelArgs*>(raw_arg);

  // Vision Transformer Patch-Embedding Tile Dimensions
  constexpr uint32_t OC = 16;
  constexpr uint32_t CKK = 768; // 3 * 16 * 16
  constexpr uint32_t P = 16;
  constexpr uint32_t PAD = 16; // Padding to avoid 16-way bank conflicts (16 floats = 64 bytes)
  constexpr uint32_t TOTAL_ELEMENTS = 12288; // OC * CKK = 3 * 64 * 64
  constexpr uint32_t smem_w_offset = 0;
  constexpr uint32_t smem_a_offset = OC * CKK * 4; // Place input after weights

  // === Localized Staging Phase ===
  {
    // Interleaved staging loop
    for (uint32_t i = tid_in_threadblock; i < TOTAL_ELEMENTS; i += threads_per_threadblock) {
      // Issue global loads first to hide latency
      uint32_t w_data = __builtin_bit_cast(uint32_t, a->w[i]);
      uint32_t in_data = __builtin_bit_cast(uint32_t, a->in[i]);
      
      // Compute weight address
      uint32_t w_addr = smem_w_offset + i * 4;
      
      // Compute input address
      {
        // Bitwise decompose flat index `i`
        // Layout: [c][h][w] = [2 bits][6 bits][6 bits]
        const uint32_t c = i >> 12;
        const uint32_t h = (i >> 6) & 0x3F;
        const uint32_t w = i & 0x3F;

        // Determine patch coordinates and kernel offsets
        const uint32_t p_h = h >> 4; // h / 16
        const uint32_t p_w = w >> 4; // w / 16
        const uint32_t kh = h & 0xF; // h % 16
        const uint32_t kw = w & 0xF; // w % 16

        // Map to patch index [0..15] and flattened ckk index [0..767]
        const uint32_t p = (p_h << 2) | p_w; // p_h * 4 + p_w
        const uint32_t c_kh_kw = (c << 8) | (kh << 4) | kw; // c * 256 + kh * 16 + kw

        // Calculate padded SMEM address: each patch occupies (768 + PAD) floats
        const uint32_t smem_addr = smem_a_offset + ((p * (CKK + PAD)) + c_kh_kw) * 4;
        
        // Store inputs first to potentially overlap with weight store
        store_shared(smem_addr, 0, in_data);
      }
      
      // Store weights
      store_shared(w_addr, 0, w_data);
    }
  } // End of staging scope - all staging temporaries are dead here

  // Cross-Core Barrier (ID 0): Ensure both cores finish SMEM staging
  mu_barrier(0, threads_per_threadblock / MU_NUM_THREADS);

  // Frugal rolled GEMM with single accumulator and conflict-free SMEM access
  for (uint32_t idx = tid_in_threadblock; idx < OC * P; idx += threads_per_threadblock) {
    const uint32_t oc = idx >> 4;
    const uint32_t p = idx & 0xF;

    uint32_t w_ptr = smem_w_offset + oc * CKK * 4;
    uint32_t a_ptr = smem_a_offset + p * (CKK + PAD) * 4;

    float acc = 0.0f;

    #pragma GCC unroll 2
    for (uint32_t k = 0; k < CKK; ++k) {
      float v_w = __builtin_bit_cast(float, load32_shared(w_ptr));
      float v_a = __builtin_bit_cast(float, load32_shared(a_ptr));
      acc += v_w * v_a;
      w_ptr += 4;
      a_ptr += 4;
    }

    a->out[idx] = acc;
  }
}
