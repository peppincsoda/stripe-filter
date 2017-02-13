#ifndef _SF_MODBUS_H_
#define _SF_MODBUS_H_

#include <sfio/sf_io_header.h>

#include <stdint.h>
#include <stddef.h>

/* Create MODBUS FC6 message (Preset Single Register) into `buffer`. */
SF_IO_API
void
sf_modbus_preset_single_register (uint8_t *buffer,
                                  size_t buffer_len,
                                  uint8_t slave_address,
                                  uint16_t data_address,
                                  uint16_t value);

/* Create MODBUS FC16 message (Preset Multiple Registers) into `buffer`. */
SF_IO_API
void
sf_modbus_preset_multiple_registers (uint8_t *buffer,
                                     size_t buffer_len,
                                     uint8_t slave_address,
                                     uint16_t data_address,
                                     uint16_t num_registers,
                                     uint16_t *values);

#endif  //  _SF_MODBUS_H_
