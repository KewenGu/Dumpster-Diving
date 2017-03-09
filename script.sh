
echo "The script starts now."
STARTTIME=$(date +%s.%N)
for i in {1..100} ; do
  rename test /tmp/test
  rename /tmp/test test
done
ENDTIME=$(date +%s.%N)
TIMEVAL=$(echo "($ENDTIME-$STARTTIME)/200.0" | bc -l)
echo "It takes $TIMEVAL seconds to complete this task."

FILESIZE=$(cat test/testfile | wc -c)
THROUGHPUT=$(echo "$FILESIZE/$TIMEVAL" | bc -l)
echo "The throughput is $THROUGHPUT bytes/sec"
echo "End script."
