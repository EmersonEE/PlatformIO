import pyaudio as pa
import struct
import numpy as np
import scipy.fftpack as fourier
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import csv

FRAMES = 1024 * 8
FORMAT = pa.paInt16
CHANNELS = 1
Fs = 44100
Fmin, Fmax = 1, 5000
DURACION_SEGUNDOS = 30


MAX_ITERACIONES = (DURACION_SEGUNDOS * Fs) // FRAMES


p = pa.PyAudio()
stream = p.open(
    format=FORMAT,
    channels=CHANNELS,
    rate=Fs,
    input=True,
    output=True,
    frames_per_buffer=FRAMES
)


fig, (ax_time, ax_freq) = plt.subplots(2, figsize=(10, 6))

x_audio = np.arange(0, FRAMES)
line_time, = ax_time.plot(x_audio, np.zeros(FRAMES), 'r')
ax_time.set_ylim(-32500, 32500)
ax_time.set_xlim(0, FRAMES)
ax_time.set_title("Señal Temporal")
ax_time.set_xlabel("Muestras")
ax_time.set_ylabel("Amplitud")

x_fft = np.linspace(0, Fs / 2, FRAMES // 2)
line_freq, = ax_freq.semilogx(x_fft, np.zeros(FRAMES // 2), 'b')
ax_freq.set_xlim(Fmin, Fmax)
ax_freq.set_ylim(0, 1) 
ax_freq.set_title("Espectro de Frecuencias")
ax_freq.set_xlabel("Frecuencia (Hz)")
ax_freq.set_ylabel("Magnitud")

F = (Fs / FRAMES) * np.arange(0, FRAMES // 2)


frecuencias_guardadas = []
contador_iteraciones = 0  

def update(frame):
    global contador_iteraciones

    try:
        
        data = stream.read(FRAMES, exception_on_overflow=False)
        data_int = struct.unpack(f"{FRAMES}h", data)

        line_time.set_ydata(data_int)

        window = np.hanning(FRAMES)
        fft_data = fourier.fft(np.array(data_int) * window) / FRAMES
        M_gk = np.abs(fft_data[:FRAMES // 2])

        line_freq.set_ydata(M_gk)
        ax_freq.set_ylim(0, np.max(M_gk) + 10)

        pos_max = np.argmax(M_gk)
        F_fund = F[pos_max]
        print(f"Frecuencia Dominante: {int(F_fund)} Hz")

        frecuencias_guardadas.append(int(F_fund))

        contador_iteraciones += 1

        if contador_iteraciones >= MAX_ITERACIONES:
            ani.event_source.stop()
            print("Toma de datos completada (15 segundos).")

    except Exception as e:
        print(f"Error: {e}")

    return line_time, line_freq


ani = FuncAnimation(fig, update, interval=10, blit=True)

plt.tight_layout()
plt.show()

def close_stream():
    stream.stop_stream()
    stream.close()
    p.terminate()

    with open("frecuencias_dominantes.csv", "w", newline='') as csvfile:
        writer = csv.writer(csvfile)
        for freq in frecuencias_guardadas:
            writer.writerow([freq])
    print("Las frecuencias dominantes han sido guardadas en 'frecuencias_dominantes.csv'.")

import atexit
atexit.register(close_stream)