from flask import Flask, render_template, request, jsonify
from braille import braille_map
import subprocess

def tocar_audio(caminho):

    processo = subprocess.Popen(
        ["mpg123", caminho]
    )

    return processo

app = Flask(__name__)


@app.route("/")
def home():
    return render_template("index.html")


@app.route("/confirmar", methods=["POST"])
def confirmar():

    dados = request.json

    letra = dados["letra"]

    tocar_audio(
    f"audios/confirmacao/letra_{letra}.mp3"
)


    pontos = braille_map[letra]


    print("Letra confirmada:", letra)

    print("Pontos Braille:", pontos)


    return jsonify({

        "status": "sucesso",

        "letra_recebida": letra,

        "pontos": pontos

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
            "status":"sucesso"
        })


    return jsonify({
        "status":"erro"
    })

@app.route("/selecionar/<letra>", methods=["POST"])
def selecionar(letra):

    processo = tocar_audio(
        f"audios/selecao/select_{letra}.mp3"
    )

    processo.wait()


    tocar_audio(
        "audios/sistema/confirmar_espaco.mp3"
    )


    return jsonify({
        "status":"sucesso"
    })

if __name__ == "__main__":
    app.run(debug=True)