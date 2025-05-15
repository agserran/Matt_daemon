⚙️ Características
Daemonización: se desliga de la terminal y corre en segundo plano.

Instancia única: crea /var/lock/matt_daemon.lock; si ya existe, detiene el arranque.

Socket TCP en 0.0.0.0:4242 usando epoll en modo Edge-Triggered con sockets non-blocking.

Límite de 3 clientes: rechaza conexiones adicionales y lo registra como error.

Logging centralizado en /var/log/matt_daemon/matt_daemon.log con etiquetas:

[ INFO ] para eventos del sistema

[ LOG ] para mensajes de cliente

[ ERROR ] para fallos y rechazos

Apagado limpio:

Detecta quit desde cualquier cliente y ejecuta shutdown.

Captura SIGTERM y SIGINT para liberar recursos y borrar lockfile.

