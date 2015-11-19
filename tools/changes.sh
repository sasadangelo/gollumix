function changes
{
    for i in $1/*
    do
        if [ -d $i ];
	    then
            changes $i $2/$i
        else
            BASENAME=`basename $i`

            if [ "$BASENAME" == "README" ];
            then
                continue
            fi

            diff $i $2/$BASENAME > /dev/null 2>&1
            RES=$?
            if [ $RES == 1 ];
            then
                echo $1/$BASENAME
            elif [ $RES == 2 ];
            then
                echo $1/$BASENAME "(new)"
            fi
        fi
    done
}

if [ $# != 1 ];
then
    echo "Usage: tools/changes.sh <directory>"
    exit 1
fi

if [ ! -d $1 ];
then
    echo "Error: the argument is not a directory"
    echo
    echo "Usage: tools/changes.sh <directory>"
    exit 2
fi

changes . $1
