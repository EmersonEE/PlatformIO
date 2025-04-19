from IPython.display import Image
import numpy as np
import scipy.fftpack as fourier
import matplotlib.pyplot as plt
import scipy.io.wavfile as waves
#import winsound
import sounddevice 


gn = [0,1,2,3,4]
gk = fourier.fft(gn)
print(gk)

M_gk = abs(gk)
Ph_gk = np.angle(gk)
print('Magnitud: ', M_gk)
print('Angle: ', Ph_gk*180/np.pi)

Ts = 0.001
Fs = 1/Ts
w1 = 2*np.pi*60
w2 = 2*np.pi*223

n = Ts*np.arange(0,1000)
ruido = np.random.random(len(n))
x = 3*np.sin(w1*n)+2.3*np.sin(w2*n)+ruido

plt.plot(n,x,'.-')
plt.xlabel('Tiempo (s)')
plt.table('Amplitud')
plt.show()

gk = fourier.fft(x)
M_gk = abs(gk)

F = Fs*np.arange(0,len(x))/len(x)

plt.plot(F,M_gk)
plt.xlabel('Frecuencia (Hz)')
plt.ylabel('Amplitud FFT')

plt.show()