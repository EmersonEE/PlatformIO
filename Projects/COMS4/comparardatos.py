import pyaudio as pa
import struct
import numpy as np
import scipy.fftpack as fourier
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import pandas as pd  # Para trabajar con el CSV

# Configuración de audio
FRAMES = 1024 * 8
FORMAT = pa.paInt16
CHANNELS = 1
Fs = 44100
Fmin, Fmax = 1, 5000
DURACION_SEGUNDOS = 30
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

# Configuración de gráficos
fig, (ax_time, ax_freq) = plt.subplots(2, figsize=(10, 6))

# Gráfico temporal
x_audio = np.arange(0, FRAMES)
line_time, = ax_time.plot(x_audio, np.zeros(FRAMES), 'r')
ax_time.set_ylim(-32500, 32500)
ax_time.set_xlim(0, FRAMES)
ax_time.set_title("Señal Temporal")
ax_time.set_xlabel("Muestras")
ax_time.set_ylabel("Amplitud")

# Gráfico de frecuencias
x_fft = np.linspace(0, Fs/2, FRAMES//2)
line_freq, = ax_freq.semilogx(x_fft, np.zeros(FRAMES//2), 'b')
ax_freq.set_xlim(Fmin, Fmax)
ax_freq.set_ylim(0, 1)
ax_freq.set_title("Espectro de Frecuencias")
ax_freq.set_xlabel("Frecuencia (Hz)")
ax_freq.set_ylabel("Magnitud")

F = (Fs/FRAMES) * np.arange(0, FRAMES//2)

# Lista para almacenar frecuencias dominantes
frecuencias_dominantes = []
contador_iteraciones = 0

def update(frame):
    global contador_iteraciones
    
    try:
        # Leer datos de audio
        data = stream.read(FRAMES, exception_on_overflow=False)
        data_int = struct.unpack(f"{FRAMES}h", data)
        
        # Actualizar gráfico temporal
        line_time.set_ydata(data_int)
        
        # Calcular FFT
        window = np.hanning(FRAMES)
        fft_data = fourier.fft(np.array(data_int) * window) / FRAMES
        M_gk = np.abs(fft_data[:FRAMES//2])
        
        # Actualizar gráfico de frecuencias
        line_freq.set_ydata(M_gk)
        ax_freq.set_ylim(0, np.max(M_gk)+10)
        
        # Encontrar frecuencia dominante
        pos_max = np.argmax(M_gk)
        F_fund = F[pos_max]
        print(f"Frecuencia Dominante: {int(F_fund)} Hz")
        
        # Guardar en lista
        frecuencias_dominantes.append(int(F_fund))
        
        contador_iteraciones += 1
        
        if contador_iteraciones >= MAX_ITERACIONES:
            ani.event_source.stop()
            print("Toma de datos completada.")
            # Al terminar, comparar con el CSV
            comparar_con_csv(frecuencias_dominantes)
            
    except Exception as e:
        print(f"Error: {e}")
    
    return line_time, line_freq

def comparar_con_csv(frecuencias):
    """Función para comparar las frecuencias con el archivo CSV sin importar posición"""
    try:
        # Leer el archivo CSV
        df = pd.read_csv('/home/emerson/Documentos/PlatformIO/Projects/COMS4/frecuencias_dominantesVelocidad3.csv')  # Cambia al nombre de tu archivo
        
        print("\n=== Resultados de comparación (sin importar posición) ===")
        print(f"Frecuencias capturadas: {len(frecuencias)} valores únicos")
        
        # Convertir a conjunto para comparación más eficiente
        frecuencias_set = set(frecuencias)
        
        # Comparar con cada columna
        for columna in df.columns:
            col_data = df[columna].dropna().values  # Obtener datos de la columna
            col_set = set(col_data)
            
            # Valores en común
            comunes = frecuencias_set & col_set
            num_comunes = len(comunes)
            porcentaje = (num_comunes / len(frecuencias_set)) * 100
            
            print(f"\nColumna '{columna}':")
            print(f"- Valores coincidentes: {num_comunes}/{len(frecuencias_set)} ({porcentaje:.2f}%)")
            
            # Opcional: Mostrar los valores que coinciden (los primeros 10 para no saturar)
            if comunes:
                print(f"- Algunos valores comunes: {sorted(comunes)[:10]}{'...' if len(comunes)>10 else ''}")
            else:
                print("- No hay valores coincidentes")
                
            # Estadísticas adicionales (opcional)
            if num_comunes > 0:
                min_comun = min(comunes)
                max_comun = max(comunes)
                print(f"- Rango de valores comunes: {min_comun} Hz a {max_comun} Hz")
                
    except Exception as e:
        print(f"\nError al comparar con CSV: {e}")

# Configurar animación
ani = FuncAnimation(fig, update, interval=10, blit=True)
plt.tight_layout()
plt.show()

def close_stream():
    """Función para cerrar el stream de audio"""
    stream.stop_stream()
    stream.close()
    p.terminate()
    print("\nStream de audio cerrado correctamente.")
    
    # Mostrar las frecuencias capturadas
    print(f"\nFrecuencias capturadas ({len(frecuencias_dominantes)}):")
    print(frecuencias_dominantes)

# Registrar función de cierre
import atexit
atexit.register(close_stream)