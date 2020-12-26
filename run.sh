perl separator_big5.pl ./test_data/example.txt > ./test_data/example_seg.txt
perl separator_big5.pl ./corpus.txt > ./corpus_seg.txt
python3 src/mapping.py ./Big5-ZhuYin.map ./ZhuYin-Big5.map
ngram-count -text corpus_seg.txt -write lm.cnt -order 2
ngram-count -read lm.cnt -lm bigram.lm -order 2 -unk
disambig -text ./test_data/example_seg.txt -map ./ZhuYin-Big5.map -lm bigram.lm -order 2 > ./test_data/example_out.txt
ngram-count -text corpus_seg.txt -write lm_tri.cnt -order 3
ngram-count -read lm_tri.cnt -lm trigram.lm -order 3 -unk
disambig -text ./test_data/example_seg.txt -map ./ZhuYin-Big5.map -lm trigram.lm -order 3 > ./test_data/example_out_tri.txt