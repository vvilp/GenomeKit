PATH=$PATH:"/Volumes/Macintosh HD New/source/jellyfish-2.2.0/bin/"

#find . -type f -exec jellyfish count -m 10 -s 100M -t 10 -C "{}" -o "{}" \;
#find . -type f -exec echo "{}" \;

find ./Genomes -name '*.fna' -mindepth 1  -type f |

while read FILEDIR #Read all files line by line.do
do
    DIR="${FILEDIR%/*}"                   # Get the folder name its inside
    FILE="${FILEDIR/*\/}"                 # Get the plain file name.

    # echo ${DIR}
    # INPUT=${DIR}
    # SUBSTRING=`echo $INPUT| cut -d'_' -f 1`
    echo $FILEDIR
    cat $FILEDIR >> all.fna

done
