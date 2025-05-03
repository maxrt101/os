#include <x86_64.h>

/** PIC 1 */
#define PIC_IRQ_TIMER           0
#define PIC_IRQ_KEYBOARD        1
#define PIC_IRQ_SERIAL2         3
#define PIC_IRQ_SERIAL1         4
#define PIC_IRQ_PARALLEL2       5
#define PIC_IRQ_DISKETTE        6
#define PIC_IRQ_PARALLEL1       7

/** PIC 2 */
#define PIC_IRQ_CMOSTIMER       0
#define PIC_IRQ_CGARETRACE      1
#define PIC_IRQ_AUXILIARY       4
#define PIC_IRQ_FPU             5
#define PIC_IRQ_HDC             6

#define PIC_EOI       0x20

/** PIC 1 Register Port Addresses */
#define PIC1_REG_COMMAND        0x20 /** Command Register */
#define PIC1_REG_STATUS         0x20 /** Status Register */
#define PIC1_REG_DATA           0x21 /** Data Register */
#define PIC1_REG_IMR            0x21 /** Interrupt Mask Register */

/** PIC 2 Register Port Addresses */
#define PIC2_REG_COMMAND        0xA0 /** Command Register */
#define PIC2_REG_STATUS         0xA0 /** Status Register */
#define PIC2_REG_DATA           0xA1 /** Data Register */
#define PIC2_REG_IMR            0xA1 /** Interrupt Mask Register */

/** Command Control Words For ICW1 */
#define PIC_ICW1_IC4_EXPECT     1
#define PIC_ICW1_IC4_NO         0
#define PIC_ICW1_SNGL_YES       2
#define PIC_ICW1_SNGL_NO        0
#define PIC_ICW1_ADI_CALLINT4   4
#define PIC_ICW1_ADI_CALLINT8   0
#define PIC_ICW1_LTIM_LEVELTRIG 8
#define PIC_ICW1_LTIM_EDGETRIG  0
#define PIC_ICW1_INIT_YES       0x10
#define PIC_ICW1_INIT_NO        0

/** Initialization Control Word 4 bit masks */
#define PIC_ICW4_MASK_UPM       0x1 /** Mode */
#define PIC_ICW4_MASK_AEOI      0x2 /** Auto EOI */
#define PIC_ICW4_MASK_MS        0x4
#define PIC_ICW4_MASK_BUF       0x8
#define PIC_ICW4_MASK_SFNM      0x10

/** Command Control Words For ICW4 */
#define PIC_ICW4_UPM_86MODE     1
#define PIC_ICW4_UPM_MSCMODE    0
#define PIC_ICW4_AEOI_AUTO      2
#define PIC_ICW4_AEOI_NOAUTO    0
#define PIC_ICW4_MS_BUFFMASTER  4
#define PIC_ICW4_MS_BUFFSLAVE   0
#define PIC_ICW4_BUF_MODEYES    8
#define PIC_ICW4_BUF_MODENO     0
#define PIC_ICW4_SFNM_NESTED    0x10
#define PIC_ICW4_SFNM_NOTNESTED 0

static struct {
  uint8_t base;
  uint8_t mask;
} pic0, pic1;

__STATIC_INLINE void pic_send_cmd(uint8_t pic, uint8_t cmd) {
  outb(pic == 1 ? PIC2_REG_COMMAND : PIC1_REG_COMMAND, cmd);
}

__STATIC_INLINE void pic_send_data(uint8_t pic, uint8_t data) {
  outb(pic == 1 ? PIC2_REG_DATA : PIC1_REG_DATA, data);
}

void x86_64_init_pic(uint8_t base0, uint8_t base1) {
  pic0.base = base0;
  pic0.mask = 0xFF;
  pic1.base = base1;
  pic1.mask = 0xFF;

  pic_send_cmd(0, PIC_ICW1_INIT_YES | PIC_ICW1_IC4_EXPECT);
  pic_send_cmd(1, PIC_ICW1_INIT_YES | PIC_ICW1_IC4_EXPECT);

  pic_send_data(0, base0);
  pic_send_data(1, base1);

  pic_send_data(0, 0x4);
  pic_send_data(1, 0x2);

  pic_send_data(0, PIC_ICW4_UPM_86MODE);
  pic_send_data(1, PIC_ICW4_UPM_86MODE);

  pic_send_data(0, 0xFF);
  pic_send_data(1, 0xFF);
}

void x86_64_irq_unmask(uint64_t n) {
  if (n < pic1.base) {
    pic0.mask &= ~(1 << ((n - pic0.base) % 8));
    pic_send_data(0, pic0.mask);
  } else {
    pic1.mask &= ~(1 << ((n - pic1.base) % 8));
    pic_send_data(1, pic1.mask);
  }
}

void x86_64_irq_mask(uint64_t n) {
  if (n < pic1.base) {
    pic0.mask |= (1 << ((n - pic0.base) % 8));
    pic_send_data(0, pic0.mask);
  } else {
    pic1.mask |= (1 << ((n - pic1.base) % 8));
    pic_send_data(1, pic1.mask);
  }
}

void x86_64_irq_done(uint64_t n) {
  if (n >= pic1.base) {
    pic_send_cmd(1, PIC_EOI);
  }

  pic_send_cmd(0, PIC_EOI);
}
