from flask import Flask, render_template, request, jsonify
from audio import tocar_audio, tocar_sequencia
import serial
import time

app = Flask(__name__)

ESP_PORT = "/dev/ttyACM0"  # Substitua pelo caminho correto da porta serial do ESP.
ESP_BAUDRATE = 115200
esp = None


def obter_esp():
    global esp

    if esp is not None and esp.is_open:
        return esp

    try:
        esp = serial.Serial(ESP_PORT, ESP_BAUDRATE, timeout=1)
        time.sleep(2)
        return esp
    except serial.SerialException as erro:
        print(f"Nao foi possivel conectar ao ESP em {ESP_PORT}: {erro}")
        return None

@app.route("/")
def home():
    return render_template("index.html")


@app.route("/confirmar", methods=["POST"])
def confirmar():

    print("=== ENTREI NA ROTA CONFIRMAR ===")

    dados = request.json
    letra = dados["letra"]

    print("Letra:", letra)

    tocar_audio(f"audios/confirmacao/letra_{letra}.mp3")

    esp_atual = obter_esp()
    if esp_atual is None:
        return jsonify({
            "status": "erro",
            "mensagem": "ESP nao conectado"
        }), 503

    # Abaixa a letra anterior
    esp_atual.write(b"RESET\n")
    esp_atual.flush()

    time.sleep(0.5)

    # Forma a nova letra
    esp_atual.write(f"{letra}\n".encode())
    esp_atual.flush()

    print("Enviei!")

    return jsonify({
        "status": "sucesso"
    })

@app.route("/audio/<nome>", methods=["POST"])
def audio(nome):

    arquivos = {
        "invalida": "audios/sistema/tecla_invalida.mp3",
        "nenhuma": "audios/sistema/nenhuma_letra.mp3",
        "nova": "audios/sistema/nova_letra.mp3",
        "espaco": "audios/sistema/confirmar_espaco.mp3"
    }

    if nome in arquivos:
        tocar_audio(arquivos[nome])

        return jsonify({
            "status": "sucesso"
        })

    return jsonify({
        "status": "erro"
    })

@app.route("/selecionar/<letra>", methods=["POST"])
def selecionar(letra):

    tocar_sequencia([

        f"audios/selecao/select_{letra}.mp3",

        "audios/sistema/confirmar_espaco.mp3"

    ])

    return jsonify({
        "status":"sucesso"
    })

if __name__ == "__main__":
    app.run(debug=True)
