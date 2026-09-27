#!/usr/bin/env bash
# ~/.config/omarchy/hooks/battery-low.d/50-esp32-battery.sh
# Disparado automaticamente por Omarchy al detectar bateria baja ($1 = nivel)
exec "$HOME/.local/bin/omarchy-leds" battery-low "$@"
