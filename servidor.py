from flask import Flask, request, jsonify
import numpy as np
import cv2
from ultralytics import YOLO

# ==============================
# CONFIGURAÇÃO DO SERVIDOR
# ==============================
app = Flask(__name__)

# Carrega modelo YOLO (leve e rápido)
modelo = YOLO("yolov8n.pt")


# ==============================
# FUNÇÃO DE DETECÇÃO
# ==============================
def detectar_pessoa(imagem):
    resultados = modelo(imagem)

    for resultado in resultados:
        for caixa in resultado.boxes:
            classe_id = int(caixa.cls[0])
            nome_classe = modelo.names[classe_id]

            if nome_classe == "person":
                return True

    return False


# ==============================
# ROTA PRINCIPAL
# ==============================
@app.route('/detectar', methods=['POST'])
def rota_detectar():
    arquivo = request.files['imagem'].read()

    # Converter imagem recebida
    imagem_np = np.frombuffer(arquivo, np.uint8)
    imagem = cv2.imdecode(imagem_np, cv2.IMREAD_COLOR)

    # Detectar pessoa
    pessoa_detectada = detectar_pessoa(imagem)

    return jsonify({
        "pessoa": pessoa_detectada
    })


# ==============================
# INICIAR SERVIDOR
# ==============================
if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000)
