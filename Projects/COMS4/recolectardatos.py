import pyaudio as pa
import struct
import numpy as np
import scipy.fftpack as fourier
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import pandas as pd
import atexit
import os

# Configuración de audio
FRAMES = 1024 * 8
FORMAT = pa.paInt16
CHANNELS = 1
Fs = 44100
Fmin, Fmax = 1, 5000
DURACION_SEGUNDOS = 30

# Cálculo de iteraciones máximas
MAX_ITERACIONES = (DURACION_SEGUNDOS * Fs) // FRAMES

# Inicialización de PyAudio
p = pa.PyAudio()
stream = p.open(
    format=FORMAT,
    channels=CHANNELS,
    rate=Fs,
    input=True,
    output=True,
    frames_per_buffer=FRAMES
)

# Configuración del gráfico
fig, (ax_time, ax_freq) = plt.subplots(2, figsize=(10, 6))

# Configuración del gráfico temporal
x_audio = np.arange(0, FRAMES)
line_time, = ax_time.plot(x_audio, np.zeros(FRAMES), 'r')
ax_time.set_ylim(-32500, 32500)
ax_time.set_xlim(0, FRAMES)
ax_time.set_title("Señal Temporal")
ax_time.set_xlabel("Muestras")
ax_time.set_ylabel("Amplitud")

# Configuración del gráfico de frecuencia
x_fft = np.linspace(0, Fs / 2, FRAMES // 2)
line_freq, = ax_freq.semilogx(x_fft, np.zeros(FRAMES // 2), 'b')
ax_freq.set_xlim(Fmin, Fmax)
ax_freq.set_ylim(0, 1) 
ax_freq.set_title("Espectro de Frecuencias")
ax_freq.set_xlabel("Frecuencia (Hz)")
ax_freq.set_ylabel("Magnitud")

# Vector de frecuencias
F = (Fs / FRAMES) * np.arange(0, FRAMES // 2)

# Variables para almacenamiento
frecuencias_guardadas = []
contador_iteraciones = 0
filename = "frecuencias_dominantesVelocidad3.csv"

def update(frame):
    global contador_iteraciones, frecuencias_guardadas

    try:
        # Lectura de datos de audio
        data = stream.read(FRAMES, exception_on_overflow=False)
        data_int = struct.unpack(f"{FRAMES}h", data)

        # Actualización del gráfico temporal
        line_time.set_ydata(data_int)

        # Cálculo de la FFT
        window = np.hanning(FRAMES)
        fft_data = fourier.fft(np.array(data_int) * window) / FRAMES
        M_gk = np.abs(fft_data[:FRAMES // 2])

        # Actualización del gráfico de frecuencia
        line_freq.set_ydata(M_gk)
        ax_freq.set_ylim(0, np.max(M_gk) + 10)

        # Cálculo de la frecuencia dominante
        pos_max = np.argmax(M_gk)
        F_fund = F[pos_max]
        print(f"Frecuencia Dominante: {int(F_fund)} Hz")

        # Almacenamiento de la frecuencia
        frecuencias_guardadas.append(int(F_fund))
        contador_iteraciones += 1

        # Detener después de alcanzar el tiempo máximo
        if contador_iteraciones >= MAX_ITERACIONES:
            ani.event_source.stop()
            print(f"Toma de datos completada ({DURACION_SEGUNDOS} segundos).")

    except Exception as e:
        print(f"Error: {e}")

    return line_time, line_freq

# Función para guardar los datos al cerrar
def close_stream():
    global frecuencias_guardadas
    
    # Detener y cerrar el stream de audio
    stream.stop_stream()
    stream.close()
    p.terminate()
    
    # Procesamiento de datos con pandas
    try:
        # Intentar leer el archivo existente
        if os.path.exists(filename):
            df_existing = pd.read_csv(filename)
            # Crear un nombre para la nueva columna
            new_col_name = f"Ejecucion_{len(df_existing.columns) + 1}"
            # Crear un DataFrame con los nuevos datos
            df_new = pd.DataFrame({new_col_name: frecuencias_guardadas})
            # Combinar con los datos existentes
            df_combined = pd.concat([df_existing, df_new], axis=1)
        else:
            # Si no existe el archivo, crear uno nuevo
            df_combined = pd.DataFrame({"Ejecucion_1": frecuencias_guardadas})
        
        # Guardar el DataFrame combinado
        df_combined.to_csv(filename, index=False)
        print(f"Datos guardados en '{filename}' con {len(df_combined.columns)} columnas.")
        
    except Exception as e:
        print(f"Error al guardar los datos: {e}")

# Registrar la función de cierre
atexit.register(close_stream)

# Iniciar la animación
ani = FuncAnimation(fig, update, interval=10, blit=True)

plt.tight_layout()
plt.show()