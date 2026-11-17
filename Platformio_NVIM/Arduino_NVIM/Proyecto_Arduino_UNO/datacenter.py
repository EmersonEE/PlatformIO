#!/usr/bin/env python3
# =========================================================================
# EQUIVALENTE LINUX DE datacenter.ttl (MACRO TERATERM)
# -------------------------------------------------------------------------
# La macro .ttl original requiere Tera Term, que es exclusivo de Windows.
# Este script reproduce exactamente la misma funcionalidad en Linux:
#   - Conexion a un puerto serie (9600 baudios, 8-N-1).
#   - Lectura no bloqueada de lineas JSON delimitadas por \n.
#   - Estampado de fecha/hora local en formato ISO 8601 (YYYY-MM-DDTHH:MM:SS).
#   - Registro en 'datacenter.log' en modo append con flush+fsync inmediato,
#     para no perder tramas si se corta la alimentacion de la estacion.
#
# USO BASICO:
#   python3 datacenter.py
#   python3 datacenter.py -p /dev/ttyACM0          # otro puerto
#   python3 datacenter.py -p /dev/ttyUSB0 -b 115200 -l auditoria.log
#
# DEPENDENCIAS:
#   python3 -m pip install pyserial   (ya instalado en este equipo)
# =========================================================================

import argparse
import os
import sys
from datetime import datetime

import serial


def marca_de_tiempo() -> str:
    """Timestamp local ISO 8601, identico al getdate/gettime de la macro."""
    return datetime.now().isoformat(timespec="seconds")


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Ingesta de telemetria JSON del Data Center (equivalente a datacenter.ttl)."
    )
    parser.add_argument(
        "-p", "--puerto", default="/dev/ttyUSB0",
        help="Puerto serie (por defecto: /dev/ttyUSB0)",
    )
    parser.add_argument(
        "-b", "--baudios", type=int, default=9600,
        help="Velocidad en baudios (por defecto: 9600, igual que la macro)",
    )
    parser.add_argument(
        "-l", "--log", default="datacenter.log",
        help="Archivo de auditoria (por defecto: datacenter.log)",
    )
    args = parser.parse_args()

    try:
        puerto = serial.Serial(args.puerto, args.baudios, timeout=None)
    except serial.SerialException as error:
        print(
            f"Error critico: no se pudo conectar a {args.puerto} a {args.baudios} baudios.\n"
            f"  Detalle: {error}\n"
            f"  Verifique: cable USB conectado, que el puerto exista (ls /dev/ttyUSB* /dev/ttyACM*)\n"
            f"  y que su usuario tenga permisos sobre el dispositivo.",
            file=sys.stderr,
        )
        return 1

    print(
        f"Escuchando {args.puerto} a {args.baudios} 8-N-1 -> {args.log}\n"
        f"Ctrl+C para finalizar la sesion.",
        flush=True,
    )

    with puerto, open(args.log, "a", encoding="utf-8") as archivo_log:
        # Cabecera de auditoria al arrancar la sesion (igual que la macro)
        archivo_log.write(
            f"{marca_de_tiempo()} [AUDITORIA] INICIO DE SESION DE TELEMETRIA DATA CENTER\n"
        )
        archivo_log.flush()
        os.fsync(archivo_log.fileno())

        try:
            while True:
                # readline() espera una linea completa terminada en LF (equivale a recvln)
                datos = puerto.readline()
                if not datos:
                    # b'' significa fin de flujo: el puerto se cerro, no hay que
                    # reintentar en bucle (evita un busy-loop al 100 % de CPU).
                    print("\nConexion serie cerrada por el dispositivo.", file=sys.stderr)
                    break

                linea = datos.decode("utf-8", errors="replace").rstrip("\r\n")
                if not linea:
                    continue  # Linea vacia (solo LF): se descarta

                registro = f"{marca_de_tiempo()} {linea}"
                archivo_log.write(registro + "\n")
                archivo_log.flush()             # vaciado a disco (equivale a flushlog)
                os.fsync(archivo_log.fileno())  # garantia fisica ante corte de energia
                print(registro, flush=True)

        except KeyboardInterrupt:
            print(f"\nSesion finalizada. Telemetria registrada en: {args.log}")
            return 0
        except serial.SerialException as error:
            print(
                f"\nError: se perdio el puerto serie ({error}). "
                f"Telemetria registrada en: {args.log}",
                file=sys.stderr,
            )
            return 1


if __name__ == "__main__":
    sys.exit(main())
