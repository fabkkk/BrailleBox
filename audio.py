import subprocess

audio_atual = None

def tocar_audio(caminho):

    global audio_atual


    if audio_atual:

        audio_atual.terminate()


    audio_atual = subprocess.Popen(
        ["mpg123", caminho]
    )


    return audio_atual