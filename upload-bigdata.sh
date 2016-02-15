
SEVER="n8975698@SEF-EES-BIGDATA-$1:~/GenomeKit"
echo "$SEVER"
scp *.cpp *.hpp *.h CMakeLists.txt $SEVER

SEVER="n8975698@SEF-EES-BIGDATA-$1:~/GenomeKit/Script"
scp Script/*.py $SEVER
