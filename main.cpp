#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>

// ==============================
// CONFIGURAÇÕES WIFI
// ==============================
const char* NOME_WIFI = "SEU_WIFI";
const char* SENHA_WIFI = "SUA_SENHA";

// URL do servidor Python
const char* URL_SERVIDOR = "http://192.168.0.100:5000/detectar";


// ==============================
// CONFIGURAÇÃO DA CÂMERA
// ==============================
void configurar_camera() {
  camera_config_t config;

  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;

  config.pin_d0 = 5;
  config.pin_d1 = 18;
  config.pin_d2 = 19;
  config.pin_d3 = 21;
  config.pin_d4 = 36;
  config.pin_d5 = 39;
  config.pin_d6 = 34;
  config.pin_d7 = 35;

  config.pin_xclk = 0;
  config.pin_pclk = 22;
  config.pin_vsync = 25;
  config.pin_href = 23;

  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;

  config.pin_pwdn = 32;
  config.pin_reset = -1;

  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 10;
  config.fb_count = 1;

  esp_camera_init(&config);
}


// ==============================
// CONECTAR WIFI
// ==============================
void conectar_wifi() {
  WiFi.begin(NOME_WIFI, SENHA_WIFI);

  Serial.print("Conectando ao WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
}


// ==============================
// ENVIAR IMAGEM PARA O SERVIDOR
// ==============================
bool enviar_imagem_para_servidor(camera_fb_t *frame) {
  HTTPClient http;

  http.begin(URL_SERVIDOR);
  http.addHeader("Content-Type", "image/jpeg");

  int codigo_resposta = http.POST(frame->buf, frame->len);

  if (codigo_resposta > 0) {
    String resposta = http.getString();
    Serial.println(resposta);

    if (resposta.indexOf("true") != -1) {
      http.end();
      return true;
    }
  }

  http.end();
  return false;
}


// ==============================
// SETUP
// ==============================
void setup() {
  Serial.begin(115200);

  conectar_wifi();
  configurar_camera();
}


// ==============================
// LOOP PRINCIPAL
// ==============================
void loop() {
  camera_fb_t *frame = esp_camera_fb_get();

  if (!frame) {
    Serial.println("Erro ao capturar imagem");
    return;
  }

  bool pessoa_detectada = enviar_imagem_para_servidor(frame);

  if (pessoa_detectada) {
    Serial.println("Pessoa detectada!");
  } else {
    Serial.println("Nenhuma pessoa detectada.");
  }

  esp_camera_fb_return(frame);

  delay(5000);
}
