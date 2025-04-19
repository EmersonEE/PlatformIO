import numpy as np
import scipy.fftpack as fourier
import matplotlib.pyplot as plt
import scipy.io.wavfile as waves
import sounddevice as sd

# Cargar el archivo de audio
filename = '/home/emerson/Descargas/velocidad1'
Fs, data = waves.read(filename)

Audio_m = data[:, 0] if len(data.shape) == 2 else data

L = len(Audio_m)
n = np.arange(0, L) / Fs

gk = fourier.fft(Audio_m)
M_gk = np.abs(gk)
M_gk = M_gk[:L // 2]
Ph_gk = np.angle(gk)
F = Fs * np.arange(0, L // 2) / L

Posm = np.argmax(M_gk)
F_fund = F[Posm]

print(f'Frecuencia Máxima: {F_fund} Hz')

fig, axs = plt.subplots(2, 1, figsize=(10, 8))

axs[0].plot(n, Audio_m)
axs[0].set_xlabel('Tiempo [s]')
axs[0].set_ylabel('Amplitud')
axs[0].set_title('Señal de Audio en el Dominio del Tiempo')

axs[1].plot(F, M_gk)
axs[1].set_xlabel('Frecuencia [Hz]')
axs[1].set_ylabel('Amplitud FFT')
axs[1].set_title('Espectro de Frecuencia (FFT)')

plt.tight_layout()
plt.show()

sd.play(data, Fs)
sd.wait()
