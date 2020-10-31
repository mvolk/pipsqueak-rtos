idf_component_register(SRCS "mqtt_client.c" "psq4_aws_iot.c"
                       INCLUDE_DIRS "include"
                       REQUIRES "esp-aws-iot"
                       PRIV_REQUIRES "fatfs" "nvs_flash" "psq4-system")
target_add_binary_data(${COMPONENT_TARGET} "certs/aws-root-ca.pem" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "certs/certificate.pem.crt" TEXT)
target_add_binary_data(${COMPONENT_TARGET} "certs/private.pem.key" TEXT)