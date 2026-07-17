import subprocess
import threading

processo = None
thread_audio = None
cancelar = threading.Event()
lock = threading.Lock()


def _executar(lista):

    global processo

    cancelar.clear()

    for arquivo in lista:

        if cancelar.is_set():
            return

        with lock:

            processo = subprocess.Popen(
                ["mpg123", arquivo]
            )

        processo.wait()

        if cancelar.is_set():
            return


def tocar_audio(arquivo):

    tocar_sequencia([arquivo])


def tocar_sequencia(lista):

    global processo
    global thread_audio

    cancelar.set()

    with lock:

        if processo and processo.poll() is None:

            processo.terminate()

            processo.wait()

    if thread_audio and thread_audio.is_alive():

        thread_audio.join()

    thread_audio = threading.Thread(

        target=_executar,

        args=(lista,),

        daemon=True

    )

    thread_audio.start()