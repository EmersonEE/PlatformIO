#!/usr/bin/env bash
# ~/.config/omarchy/hooks/theme-set.d/50-esp32-leds.sh
# Disparado automaticamente por Omarchy al cambiar de tema ($1 = nombre del tema)
exec "$HOME/.local/bin/omarchy-leds" sync "$@"
