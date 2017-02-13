#! /bin/sh
### BEGIN INIT INFO
# Provides:          grabc
# Required-Start:    $remote_fs $syslog
# Required-Stop:     $remote_fs $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
### END INIT INFO

# PATH should only include /usr/* if it runs after the mountnfs.sh script
DESC="grabc image processing service"
NAME=grabc
WORKDIR=/home/pi/work/sf_basler/grabc
USER=pi
LOGFILE=$WORKDIR/logs/grabc.log
DAEMON=$WORKDIR/$NAME
DAEMON_ARGS="$WORKDIR/settings.ini"
PIDFILE=/var/run/$NAME.pid
SCRIPTNAME=/etc/init.d/$NAME

export LD_LIBRARY_PATH=$WORKDIR/../sfcore:$WORKDIR/../sfio

# Exit if the package is not installed
[ -x "$DAEMON" ] || exit 0

# Read configuration variable file if it is present
[ -r /etc/default/$NAME ] && . /etc/default/$NAME

# Load the VERBOSE setting and other rcS variables
. /lib/init/vars.sh

# Define LSB log_* functions.
# Depend on lsb-base (>= 3.2-14) to ensure that this file is present
# and status_of_proc is working.
. /lib/lsb/init-functions

#
# Function that starts the daemon/service
#
do_start()
{
	# Return
	#   0 if daemon has been started
	#   1 if daemon was already running
	#   2 if daemon could not be started
	start-stop-daemon --start --quiet --chuid $USER --chdir $WORKDIR \
		--make-pidfile --pidfile $PIDFILE --background \
		--startas /bin/bash -- -c "exec $DAEMON $DAEMON_ARGS > $LOGFILE 2>&1"
	RETVAL="$?"
	[ "$RETVAL" = 0 ] && return 0
	[ "$RETVAL" = 1 ] && return 1
	return 2
}

#
# Function that stops the daemon/service
#
do_stop()
{
	# Return
	#   0 if daemon has been stopped
	#   1 if daemon was already stopped
	#   2 if a failure occurred
	start-stop-daemon --stop --quiet --user $USER --name $NAME \
		--retry TERM/30/KILL/5
	RETVAL="$?"
	rm -f $PIDFILE
	[ "$RETVAL" = 0 ] && return 0
	[ "$RETVAL" = 1 ] && return 1
	return 2
}

case "$1" in
  start)
	[ "$VERBOSE" != no ] && log_daemon_msg "Starting $DESC" "$NAME"
	do_start
	case "$?" in
		0|1) [ "$VERBOSE" != no ] && log_end_msg 0 ;;
		2) [ "$VERBOSE" != no ] && log_end_msg 1 ;;
	esac
	;;
  
  stop)
	[ "$VERBOSE" != no ] && log_daemon_msg "Stopping $DESC" "$NAME"
	do_stop
	case "$?" in
		0|1) [ "$VERBOSE" != no ] && log_end_msg 0 ;;
		2) [ "$VERBOSE" != no ] && log_end_msg 1 ;;
	esac
	;;
  
  status)
	status_of_proc "$DAEMON" "$NAME" && exit 0 || exit $?
	;;
  
  restart)
	log_daemon_msg "Restarting $DESC" "$NAME"
	do_stop
	case "$?" in
	  0|1)
		do_start
		case "$?" in
			0) log_end_msg 0 ;;
			1) log_end_msg 1 ;; # Old process is still running
			*) log_end_msg 1 ;; # Failed to start
		esac
		;;
	  *)
		# Failed to stop
		log_end_msg 1
		;;
	esac
	;;
  *)
	echo "Usage: $SCRIPTNAME {start|stop|status|restart}" >&2
	exit 3
	;;
esac

:
