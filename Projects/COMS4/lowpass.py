import serial
import time
import csv
from datetime import datetime
from pynput import keyboard  # Alternativa que no requiere root

# Configuración
PORT = '/dev/ttyUSB0'  # Puerto serial (ajusta según tu sistema)
BAUDRATE = 115200
RECORD_TIME = 5  # Duración de cada toma en segundos
NUM_TAKES = 5     # Número total de tomas a realizar
CSV_FILENAME = f"datos_sensor_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv"

class TakeController:
    def __init__(self):
        self.start_recording = False
        self.exit_program = False
        self.listener = None

    def on_press(self, key):
        try:
            if key == keyboard.Key.space:
                self.start_recording = True
                return False  # Detener listener
            elif key == keyboard.Key.esc:
                self.exit_program = True
                return False  # Detener listener
        except:
            pass

def record_take(ser, duration, take_num):
    """Registra una sola toma de datos"""
    data = []
    start_time = time.time()
    
    print(f"\nIniciando Toma {take_num + 1}/{NUM_TAKES} - Grabando por {duration} segundos...")
    
    while (time.time() - start_time) < duration:
        if ser.in_waiting:
            try:
                line = ser.readline().decode('utf-8').strip()
                if line:
                    timestamp = time.time() - start_time
                    value = int(line)
                    data.append((timestamp, value))
                    print(f"T: {timestamp:.2f}s - Valor: {value}", end='\r')
            except (UnicodeDecodeError, ValueError):
                continue
    
    print(f"\nToma {take_num + 1} completada. Datos capturados: {len(data)}")
    return data

def save_to_csv(filename, all_takes):
    """Guarda todas las tomas en un archivo CSV"""
    max_length = max(len(take) for take in all_takes)
    
    with open(filename, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        
        # Escribir encabezados
        headers = ['Timestamp'] + [f'Toma_{i+1}' for i in range(len(all_takes))]
        writer.writerow(headers)
        
        # Escribir datos alineados por timestamp
        for i in range(max_length):
            row = []
            # Añadir timestamp (usamos el de la primera toma como referencia)
            if i < len(all_takes[0]):
                row.append(f"{all_takes[0][i][0]:.3f}")
            else:
                row.append("")
            
            # Añadir valores de cada toma
            for take in all_takes:
                if i < len(take):
                    row.append(take[i][1])
                else:
                    row.append("")
            
            writer.writerow(row)
    
    print(f"\nTodos los datos guardados en {filename}")

def main():
    try:
        # Inicializar conexión serial
        ser = serial.Serial(PORT, BAUDRATE, timeout=1)
        ser.flushInput()
        
        print(f"\n{'='*50}")
        print(f"Grabador de Datos Serial - {NUM_TAKES} tomas de {RECORD_TIME}s")
        print(f"Archivo de salida: {CSV_FILENAME}")
        print(f"Presione 'ESPACIO' para iniciar cada toma")
        print(f"Presione 'ESC' para salir en cualquier momento")
        print(f"{'='*50}\n")
        
        all_takes = []
        controller = TakeController()
        
        for take in range(NUM_TAKES):
            print(f"\nPreparado para Toma {take + 1}/{NUM_TAKES}...")
            print("Presione ESPACIO para comenzar o ESC para cancelar")
            
            # Configurar listener de teclado
            with keyboard.Listener(on_press=controller.on_press) as listener:
                listener.join()
            
            if controller.exit_program:
                print("\nGrabación cancelada por el usuario")
                return
            
            if controller.start_recording:
                take_data = record_take(ser, RECORD_TIME, take)
                all_takes.append(take_data)
                controller.start_recording = False
            
        # Guardar todos los datos
        save_to_csv(CSV_FILENAME, all_takes)
        
    except serial.SerialException as e:
        print(f"Error de conexión serial: {e}")
    except Exception as e:
        print(f"Error inesperado: {e}")
    finally:
        if 'ser' in locals() and ser.is_open:
            ser.close()
            print("\nConexión serial cerrada.")

if __name__ == "__main__":
    # Instalar la librería pynput si no la tienes:
    # pip install pynput
    main()