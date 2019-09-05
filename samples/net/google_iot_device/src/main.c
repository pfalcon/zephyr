/* Copyright 2019 Google LLC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <zephyr.h>

#include "config.h"
#include "example_utils.h"
#include <iotc.h>
#include <iotc_jwt.h>
#include <iotc_error.h>

iotc_crypto_key_data_t iotc_connect_private_key_data;
char ec_private_key_pem[PRIVATE_KEY_BUFFER_SIZE] = {0};

void main(void) {
  printk("Example for Zephyr port.\n");

  if (0 != load_ec_private_key_pem(ec_private_key_pem,
                                   PRIVATE_KEY_BUFFER_SIZE)) {
    printk("\nApplication exiting due to private key load error.\n\n");
    return;
  }

  /* Format the key type descriptors so the client understands
     what type of key is being represented. In this case, a PEM encoded
     byte array of a ES256 key. */
  iotc_connect_private_key_data.crypto_key_signature_algorithm =
      IOTC_CRYPTO_KEY_SIGNATURE_ALGORITHM_ES256;
  iotc_connect_private_key_data.crypto_key_union_type =
      IOTC_CRYPTO_KEY_UNION_TYPE_PEM;
  iotc_connect_private_key_data.crypto_key_union.key_pem.key =
      ec_private_key_pem;

  printk("Starting GCP IoT Embedded C Client...\n");

  iotc_initialize();

  iotc_context_handle_t iotc_context = iotc_create_context();

  const uint16_t connection_timeout = 10;
  const uint16_t keepalive_timeout = 10;

  /* Generate the client authentication JWT, which will serve as the MQTT
   * password. */
  char jwt[IOTC_JWT_SIZE] = {0};
  size_t bytes_written = 0;
  iotc_state_t state = iotc_create_iotcore_jwt(
      iotc_project_id,
      /*jwt_expiration_period_sec=*/3600, &iotc_connect_private_key_data, jwt,
      IOTC_JWT_SIZE, &bytes_written);

  if (IOTC_STATE_OK != state) {
    printk("iotc_create_iotcore_jwt returned with error: %ul : %s", state, iotc_get_state_string(state));
    return;
  }

  iotc_connect(iotc_context, /*username=*/NULL, /*password=*/jwt,
               /*client_id=*/iotc_device_path, connection_timeout,
               keepalive_timeout, &on_connection_state_changed);

  iotc_events_process_blocking();

  /*  Cleanup the default context, releasing its memory */
  iotc_delete_context(iotc_context);

  /* Cleanup internal allocations that were created by iotc_initialize. */
  iotc_shutdown();
}

int load_ec_private_key_pem(char* buf_ec_private_key_pem, size_t buf_len) {
  strncpy(buf_ec_private_key_pem, iotc_private_key_pem, buf_len);
  buf_ec_private_key_pem[buf_len - 1] = 0;
  return 0;
}
