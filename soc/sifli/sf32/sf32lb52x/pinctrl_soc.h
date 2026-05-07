/*
 * Copyright (c) 2025 Core Devices LLC
 * Copyright (c) 2025 SiFli Technologies(Nanjing) Co., Ltd
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _SOC_SIFLI_SF32_SF32LB52X_PINCTRL_SOC_H_
#define _SOC_SIFLI_SF32_SF32LB52X_PINCTRL_SOC_H_

#include <stdint.h>

#include <zephyr/devicetree.h>
#include <zephyr/dt-bindings/pinctrl/sf32lb-common-pinctrl.h>
#include <zephyr/sys/util.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SF32LB pin configuration type.
 *
 * Two-word representation. Word 0 (@p pinmux) carries routing information
 * produced by SF32LB_PINMUX() / SF32LB_PINMUX_ANALOG(). Word 1 (@p cfg)
 * carries electrical configuration derived from devicetree node properties.
 * Splitting the two words eliminates the bit overlap between DS_IDX
 * (bits 11-13) and PORT (bits 12-13) that existed in the original
 * single-word encoding.
 *
 * @par pinmux word bitmap (encoded by SF32LB_PINMUX() macros):
 *   - 0-3:   Function select (FSEL), maps to HPSYS_PINMUX bits 0-3
 *   - 4-11:  Reserved (must be 0)
 *   - 12-13: Port (SF32LB_PORT_SA=0, SF32LB_PORT_PA=1)
 *   - 14-21: Pad number (0-127)
 *   - 22-23: PINR register field (0-3)
 *   - 24-31: PINR register offset (0=not used, 1-255)
 *
 * @par cfg word bitmap (encoded by Z_PINCTRL_STATE_PIN_INIT from DT properties):
 *   - 0-3:   Reserved (must be 0)
 *   - 4:     PE: pull enable, maps to HPSYS_PINMUX bit 4
 *   - 5:     PS: pull select (0=pulldown, 1=pullup), maps to HPSYS_PINMUX bit 5
 *   - 6:     IE: input enable, maps to HPSYS_PINMUX bit 6
 *   - 7:     IS: input schmitt trigger (not written; preserved from HW default)
 *   - 8:     SR: slew rate (not written; preserved from HW default)
 *   - 9-10:  DS register bits {DS0, DS1} written after DS_IDX conversion;
 *            DS0 is the high bit, default=2 (4mA)
 *   - 11-13: DS_IDX: drive-strength enum index (SW only, 0-4)
 *   - 14-31: Reserved (must be 0)
 */
typedef struct {
	uint32_t pinmux;
	uint32_t cfg;
} pinctrl_soc_pin_t;

#define SF32LB_PE_MSK BIT(4U)
#define SF32LB_PS_MSK BIT(5U)
#define SF32LB_IE_MSK BIT(6U)
#define SF32LB_DS_MSK GENMASK(10U, 9U)

/* Drive strength enum index position and mask (stored in bits 11-13) */
#define SF32LB_DS_IDX_POS 11U
#define SF32LB_DS_IDX_MSK GENMASK(13U, 11U)

/*
 * Pin configuration mask for bits that should be modified.
 * SR (slew-rate) and IS (input-schmitt) are preserved from hardware defaults.
 * DS bits (9-10) are set by driver after mA-to-register conversion.
 */
#define SF32LB_PINMUX_CFG_MSK                                                                      \
	(SF32LB_FSEL_MSK | SF32LB_PE_MSK | SF32LB_PS_MSK | SF32LB_IE_MSK | SF32LB_DS_MSK)

#define Z_PINCTRL_STATE_PIN_INIT(node_id, prop, idx)                                               \
	{                                                                                          \
		.pinmux = DT_PROP_BY_IDX(node_id, prop, idx),                                     \
		.cfg = (FIELD_PREP(SF32LB_PE_MSK,                                                  \
				   (DT_PROP(node_id, bias_pull_up) |                               \
				    DT_PROP(node_id, bias_pull_down))) |                            \
			FIELD_PREP(SF32LB_PS_MSK, DT_PROP(node_id, bias_pull_up)) |               \
			FIELD_PREP(SF32LB_IE_MSK, DT_PROP(node_id, input_enable)) |               \
			FIELD_PREP(SF32LB_DS_IDX_MSK,                                             \
				   DT_ENUM_IDX(node_id, drive_strength))),                        \
	},

#define Z_PINCTRL_STATE_PINS_INIT(node_id, prop)                                                   \
	{DT_FOREACH_CHILD_VARGS(DT_PHANDLE(node_id, prop), DT_FOREACH_PROP_ELEM, pinmux,           \
				Z_PINCTRL_STATE_PIN_INIT)}

#ifdef __cplusplus
}
#endif

#endif /* _SOC_SIFLI_SF32_SF32LB52X_PINCTRL_SOC_H_ */
