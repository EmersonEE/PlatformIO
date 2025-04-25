import serial
import tkinter as tk
from tkinter import ttk, messagebox
from collections import deque
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.figure import Figure

# Configuración del puerto serial
PORT = '/dev/ttyUSB1'  # Asegúrate que este es el puerto correcto
BAUD_RATE = 115200

# Configuración de la visualización
MAX_DATA_POINTS = 100  # Número de puntos visibles en pantalla
WINDOW_SIZE = 100      # Tamaño de la ventana visible
time_buffer = deque(maxlen=MAX_DATA_POINTS)
down_buffer = deque(maxlen=MAX_DATA_POINTS)
left_buffer = deque(maxlen=MAX_DATA_POINTS)
right_buffer = deque(maxlen=MAX_DATA_POINTS)
up_buffer = deque(maxlen=MAX_DATA_POINTS)

class LDRMonitorApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Monitor de LDRs - Modo Desplazamiento")
        
        # Configuración de la figura de Matplotlib
        self.fig = Figure(figsize=(10, 6), dpi=100)
        self.ax = self.fig.add_subplot(111)
        self.ax.set_title('Voltajes de LDRs (Desplazamiento Continuo)')
        self.ax.set_xlabel('Tiempo (muestras)')
        self.ax.set_ylabel('Voltaje (V)')
        self.ax.set_ylim(0, 5)
        self.ax.set_xlim(0, WINDOW_SIZE)  # Fijamos la ventana visible
        
        # Líneas para cada LDR
        self.line_down, = self.ax.plot([], [], label='Down (LDR1)', color='blue')
        self.line_left, = self.ax.plot([], [], label='Left (LDR2)', color='green')
        self.line_right, = self.ax.plot([], [], label='Right (LDR3)', color='red')
        self.line_up, = self.ax.plot([], [], label='Up (LDR4)', color='orange')
        self.ax.legend(loc='upper right')
        
        # Canvas de Matplotlib embebido en Tkinter
        self.canvas = FigureCanvasTkAgg(self.fig, master=self.root)
        self.canvas.draw()
        self.canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=True)
        
        # Panel de control
        control_frame = tk.Frame(self.root)
        control_frame.pack(side=tk.BOTTOM, fill=tk.X)
        
        self.start_button = ttk.Button(control_frame, text="Iniciar", command=self.start_monitoring)
        self.start_button.pack(side=tk.LEFT, padx=5, pady=5)
        
        self.stop_button = ttk.Button(control_frame, text="Detener", command=self.stop_monitoring, state=tk.DISABLED)
        self.stop_button.pack(side=tk.LEFT, padx=5, pady=5)
        
        # Control de velocidad de desplazamiento
        self.speed_label = ttk.Label(control_frame, text="Velocidad:")
        self.speed_label.pack(side=tk.LEFT, padx=5, pady=5)
        
        self.speed_scale = ttk.Scale(control_frame, from_=10, to=200, orient=tk.HORIZONTAL, 
                                    command=self.update_speed)
        self.speed_scale.set(50)  # Valor inicial
        self.speed_scale.pack(side=tk.LEFT, padx=5, pady=5, fill=tk.X, expand=True)
        
        self.quit_button = ttk.Button(control_frame, text="Salir", command=self.on_closing)
        self.quit_button.pack(side=tk.RIGHT, padx=5, pady=5)
        
        # Variables de control
        self.monitoring = False
        self.serial_connection = None
        self.update_interval = 50  # ms (se ajusta con la escala)
        self.current_position = 0  # Para el seguimiento de la posición
        
    def start_monitoring(self):
        try:
            self.serial_connection = serial.Serial(PORT, BAUD_RATE, timeout=1)
            self.monitoring = True
            self.start_button.config(state=tk.DISABLED)
            self.stop_button.config(state=tk.NORMAL)
            self.current_position = 0  # Reiniciamos la posición
            self.update_plot()
        except Exception as e:
            messagebox.showerror("Error", f"No se pudo abrir el puerto serial: {e}")
    
    def update_plot(self):
        if self.monitoring and self.serial_connection and self.serial_connection.in_waiting:
            try:
                line = self.serial_connection.readline().decode('utf-8').strip()
                down, left, right, up = map(float, line.split(','))
                
                # Almacena los datos
                self.current_position += 1
                time_buffer.append(self.current_position)
                down_buffer.append(down)
                left_buffer.append(left)
                right_buffer.append(right)
                up_buffer.append(up)
                
                # Actualiza las líneas
                self.line_down.set_data(time_buffer, down_buffer)
                self.line_left.set_data(time_buffer, left_buffer)
                self.line_right.set_data(time_buffer, right_buffer)
                self.line_up.set_data(time_buffer, up_buffer)
                
                # Ajusta los límites del eje x para el efecto de desplazamiento
                if self.current_position > WINDOW_SIZE:
                    self.ax.set_xlim(self.current_position - WINDOW_SIZE, self.current_position)
                else:
                    self.ax.set_xlim(0, WINDOW_SIZE)
                
                # Redibuja el canvas
                self.canvas.draw()
            except ValueError as e:
                print(f"Datos inválidos: {e}")
            except Exception as e:
                print(f"Error: {e}")
                self.stop_monitoring()
        
        # Programa la próxima actualización
        if self.monitoring:
            self.root.after(self.update_interval, self.update_plot)
    
    def update_speed(self, value):
        """Ajusta la velocidad de actualización/desplazamiento"""
        self.update_interval = 210 - int(float(value))  # Invertimos la escala (mayor valor = más lento)
    
    def stop_monitoring(self):
        self.monitoring = False
        self.start_button.config(state=tk.NORMAL)
        self.stop_button.config(state=tk.DISABLED)
        if self.serial_connection and self.serial_connection.is_open:
            self.serial_connection.close()
    
    def on_closing(self):
        self.stop_monitoring()
        self.root.quit()

if __name__ == "__main__":
    root = tk.Tk()
    app = LDRMonitorApp(root)
    root.protocol("WM_DELETE_WINDOW", app.on_closing)
    root.mainloop()