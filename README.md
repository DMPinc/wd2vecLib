## org
https://code.google.com/p/word2vec/

we change the path "malloc.h" -> "malloc/malloc.h" for build. 

usage: 
@lib directory
+ train midi file. (optional)
+ compile source codes ( cd lib && make sox )
+ ./soxserver <word2vec trained file>
+ ./soxclient "length/tone length/tone" 
