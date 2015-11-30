
SEVER="n8975698@SEF-EES-BIGDATA-$1:~/GenomeKit"
echo "$SEVER"
scp *.cpp *.hpp *.h CMakeLists.txt $SEVER
