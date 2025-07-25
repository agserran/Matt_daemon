#!/bin/bash

DAEMON=./Matt_daemon
LOG_DIR=/var/log/matt_daemon
LOG_FILE=$LOG_DIR/matt_daemon.log
LOCK_FILE=/var/lock/matt_daemon.lock

# 1. Crear carpeta si no existe
echo "🔧 Creando carpeta de logs si no existe..."
mkdir -p $LOG_DIR
chmod 755 $LOG_DIR

# 2. Lanzar daemon
echo "🚀 Lanzando daemon..."
$DAEMON &
echo "give it a moment to start..."
sleep 2
DAEMON_PID=$(cat /var/lock/matt_daemon.lock)

echo "🔍 Procesos Matt_daemon activos tras lanzamiento:"
ps aux | grep Matt | grep -v grep

# 3. Verificar log y lockfile
echo "📋 Comprobando log y lock..."
if [ -f "$LOCK_FILE" ]; then
    echo "✅ Lock file existe: $LOCK_FILE"
else
    echo "❌ Lock file NO existe"
fi

# 4. Verificar proceso en ejecución
if ps -p $DAEMON_PID > /dev/null; then
    echo "✅ Daemon en ejecución con PID $DAEMON_PID"
else
    echo "❌ Daemon no está en ejecución"
fi

# 5. Lanzar segunda instancia (debe fallar)
echo "🧪 Probando doble instancia..."
$DAEMON 2>/dev/null
sleep 2
if grep -q "Another daemon is already running" "$LOG_FILE"; then
	echo "✅ Segunda instancia bloqueada como se esperaba"
else
	echo "❌ Segunda instancia ejecutada (¡esto no debería pasar!)"
fi

# 6. Probar conexiones simultáneas
echo "🔌 Probando 4 conexiones netcat..."
for i in {1..4}; do
    echo "Hola $i" | nc localhost 4242 &
    sleep 1
done
sleep 2

echo "Mata el proceso Matt_daemon para limpiar:"
ps aux | grep "Matt_daemon" | grep -v grep | awk '{print $2}' | xargs kill -9

echo -e "\n🖥️  Ejecuta en una terminal nueva para ver el proceso activo:"
echo "watch -n 1 'ps aux | grep \"[M]att_daemon\"'"

echo -e "\n📜 Abre otra terminal para seguir el log en vivo:"
echo "tail -f /var/log/matt_daemon/matt_daemon.log"

echo -e "\n📂 Abre otra terminal para inspeccionar el lockfile:"
echo "watch -n 1 'ls -lah /var/lock | grep matt'"

echo -e "\n📡 Abre otra terminal para interactuar con el daemon vía netcat:"
echo "echo \"Hola\" | nc localhost 4242"
echo "echo \"quit\" | nc localhost 4242"

echo -e "\n⏳ Espera unos segundos y observa la salida de cada terminal para validar el comportamiento esperado."


echo "🧪 Test completo"
