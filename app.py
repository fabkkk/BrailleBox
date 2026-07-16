from flask import Flask, render_template, request, jsonify
from braille import braille_map
from audio import tocar_audio, tocar_sequencia
import serial
import time

app = Flask(__name__)

esp = serial.Serial(
    "/dev/ttyACM0",
    115200,
    timeout=1
)

time.sleep(2)

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

    print("Vou enviar para o ESP...")
    esp.write(f"{letra}\n".encode())
    esp.flush()
    print("Enviei!")

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
            "status":"sucesso"
        })


    return jsonify({
        "status":"erro"
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