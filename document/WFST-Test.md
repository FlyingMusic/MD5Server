## WFST-Test

使用简单的中文三句话，构建语言模型（G.fst），发音词典模型（L.fst），以及合并的模型（LG.fst和CLG.FST），生成HMM模型（H.fst），最终合并、确定、最小化得到HCLG.fst，并将其中的wfst模型进行可视化。

### 索引：

##### 1. 语料准备

##### 2. 构建语法模型（G.fst）

##### 3. 构建发音词典模型（L.fst）

##### 4. 合并发音词典与语法模型（LG.fst）

##### 5. 确定化发音词典与语法模型（det-LG.fst）

##### 6. 构建上下文模型与发音词典模型、语法模型（CLG.fst）

##### 7. 构建HMM模型（H.fst）

##### 8. 合并HMM模型，上下文模型、发音词典模型、语法模型（HCLG.fst）

------

### 1. 语料准备

#### 1.1 文本语料与发音词典

train.txt ：经过分词的中文语料

```txt
语音 识别 技术
语音 识别 算法 公式
作战 防御 工事
```

> 注：（1）第一句的和第二句均出现“语音 识别”，为观察相同“语音 识别”在模型中存储的情况；（2）第二句的”公式“和第三句的”工事“输入同音异形词，它们的的音素（包括声调）序列完全一致。

lexicon.txt：发音词典文件

```txt
!SIL SIL
<SPOKEN_NOISE> SPN
<SPOKEN_NOISE> sil
<UNK> SPN
语音 vv v3 ii in1
识别 sh ix2 b ie2
技术 j i4 sh u4
算法 s uan4 f a3
公式 g ong1 sh ix4
作战 z uo4 zh an4
防御 f ang2 vv v4
工事 g ong1 sh ix4
```

#### 1.2 语言模型（APRA）训练

执行命令统计1元词频，获取到1元语法模型，如下所示，获得2元语法模型和3元语法模型。

```
ngram-count -text train.txt -order 1 -write train-1gram.count 
ngram-count -text train.txt -order 2 -write train-2gram.count 
ngram-count -text train.txt -order 3 -write train-3gram.count 
ngram-count -read train-1gram.count -order 1 -lm train-1gram.arpa 
ngram-count -read train-2gram.count -order 2 -lm train-2gram.arpa 
ngram-count -read train-3gram.count -order 3 -lm train-3gram.arpa 
```

### 2. 构建语法模型（G.fst）

#### 2.1 arpa转fst

执行命令将arpa格式1元语法模型转换为fst格式。如下所示，获得2元语法模型和3元语法模型。

```
/home/kaldi/src/lmbin/arpa2fst --disambig-symbol=#0 --read-symbol-table=words.txt train-1gram.arpa G-1gram.fst 
/home/kaldi/src/lmbin/arpa2fst --disambig-symbol=#0 --read-symbol-table=words.txt train-2gram.arpa G-2gram.fst 
/home/kaldi/src/lmbin/arpa2fst --disambig-symbol=#0 --read-symbol-table=words.txt train-3gram.arpa G-3gram.fst 
```

#### 2.2 语法模型可视化

执行如下命令将语法模型fst文件输出为dot格式文件：

```
/home/kaldi/tools/openfst/bin/fstdraw --isymbols=words.txt --osymbols=words.txt G-1gram.fst > G-1gram.dot
/home/kaldi/tools/openfst/bin/fstdraw --isymbols=words.txt --osymbols=words.txt G-2gram.fst > G-2gram.dot 
/home/kaldi/tools/openfst/bin/fstdraw --isymbols=words.txt --osymbols=words.txt G-3gram.fst > G-3gram.dot 
```

> 注：其中–osymbols=words.txt指定了fst的输出标识，words.txt的生成见发音词典模型的构建中dict与lang文件的生成，鉴于大流程（HCLG）的统一，words.txt文件需要提前生成，请先移步3.2。

由于默认的dot文件定义的图像大小为：size:”8.5,11”，需要改变其大小，size=“20,32”。

```
digraph FST {
rankdir = LR;
size = "20,30";
label = "";
center = 1;
orientation = Landscape;
ranksep = "0.4";
nodesep = "0.25";
node [fontname=SimSun];
0 [label = "0/1.4663", shape = doublecircle, style = bold, fontsize = 14]
    0 -> 0 [label = "作战:作战/2.5649", fontsize = 14];
    0 -> 0 [label = "公式:公式/2.5649", fontsize = 14];
    0 -> 0 [label = "工事:工事/2.5649", fontsize = 14];
    0 -> 0 [label = "技术:技术/2.5649", fontsize = 14];
    0 -> 0 [label = "算法:算法/2.5649", fontsize = 14];
    0 -> 0 [label = "识别:识别/1.8718", fontsize = 14];
    0 -> 0 [label = "语音:语音/1.8718", fontsize = 14];
    0 -> 0 [label = "防御:防御/2.5649", fontsize = 14];
}
```

分别编辑并保存1元，2元和3元语法模型的dot文件，执行如下命令输入图片：

```
dot -Tjpg G-1gram.dot > G-1gram.jpg
dot -Tjpg G-2gram.dot > G-2gram.jpg 
dot -Tjpg G-3gram.dot > G-3gram.jpg 
```

语法模型图像如下：

![G-1gram](C:\work\tmp\G-1gram.jpg)



### 3. 构建发音词典模型（L.fst）

#### 3.1 手动构造dict目录

手动创建dict文件夹，生成如下7个文件：

1. 发音词典文件：lexicon.txt，复制如上所述文件。
2. 发音词典概率文件：lexiconp.txt，即在lexicon.txt文件中词语与音素序列之间添加1。

```txt
!SIL 1 SIL
<SPOKEN_NOISE> 1 SPN
<SPOKEN_NOISE> 1 sil
<UNK> 1 SPN
语音 1 vv v3 ii in1
识别 1 sh ix2 b ie2
技术 1 j i4 sh u4
算法 1 s uan4 f a3
公式 1 g ong1 sh ix4
作战 1 z uo4 zh an4
防御 1 f ang2 vv v4
工事 1 g ong1 sh ix4
```

1. 音素问题划分文件：extra_question.txt，相同声调的不同音素放在一行，分组排列。

```txt
SIL SPN
vv ii sh b j s f g z zh
in1 ong1
ix2 ie2 ang2
v3 a3
v4 ix4 i4 u4 uan4 uo4 an4
```

1. 非静音音素文件：nonsilence_phones.txt

```txt
vv
v3 v4
ii
in1
sh
ix2 ix4
b
ie2
j
i4
u4
s
uan4
f
a3
g
ong1
z
uo4
zh
an4
ang2
sil
```

1. 静音音素文件：silence_phones.txt

```
1 2 SIL SPN 
```

1. 可选静音音素文件：optional_silence.txt

```txt
SIL
```

1. 静音音素概率文件：silprob.txt，概率值来源于历史训练数据。

```txt
<s> 0.97
</s>_s 2.25287275463452
</s>_n 0.0751953756044114
overall 0.31
```

#### 3.2 lang文件夹与发音词典模型生成

使用kaldi脚本生成lang文件目录及相关文件。

```
1 2 cd /home/kaldi/egs/wsj/s5 utils/prepare_lang.sh /home/ngram-test/data/dict "<UNK>" /home/ngram-test/data/lang_tmp /home/ngram-test/data/lang 
```

在data/lang目录下的到L.fst和L_disambig.fst，其中L_disambig.fst为添加了消歧音素的发音词典模型。另外，还得到上述步骤需要的words.txt，以及后续步骤需要的topo、phones.txt、phones/disambig.int文件。

#### 3.3 发音词典模型可视化

执行如下命令将发音词典模型fst输出为dot格式文件：

```
1 2 /home/kaldi/tools/openfst/bin/fstdraw --isymbols=phones.txt --osymbols=words.txt L.fst > L.dot /home/kaldi/tools/openfst/bin/fstdraw --isymbols=phones.txt --osymbols=words.txt L_disambig.fst > L_disambig.dot 
```

修改dot文件中的的size属性：

L.dot：size = “25,40”

L_disambig.dot：size = “25,40”

执行dot命令输出图片：

```
1 2 dot -Tjpg L.dot > L.jpg dot -Tjpg L_disambig.dot > L_disambig.jpg 
```

发音词典模型图像如下：

![发音词典模型](https://wangkaisine.github.io//img/in-post/post-wfst-test/L.jpg)

![消歧发音词典模型](https://wangkaisine.github.io//img/in-post/post-wfst-test/L_disambig.jpg)

### 4. 合并发音词典与语法模型（LG.fst）

#### 4.1 LdG.fst模型生成

执行如下命令将L_disambig.fst分别与G-Ngram.fst合并，输出合并的LdG-Ngram.fst模型，同样的，可以将L.fst分别于G-Ngram.fst合并，得到对应的LG-Ngram.fst。

```
1 2 3 4 5 6 /home/kaldi/tools/openfst/bin/fstcompose L_disambig.fst G-1gram.fst LdG-1gram.fst /home/kaldi/tools/openfst/bin/fstcompose L_disambig.fst G-2gram.fst LdG-2gram.fst /home/kaldi/tools/openfst/bin/fstcompose L_disambig.fst G-3gram.fst LdG-3gram.fst /home/kaldi/tools/openfst/bin/fstcompose L.fst G-1gram.fst LG-1gram.fst /home/kaldi/tools/openfst/bin/fstcompose L.fst G-2gram.fst LG-2gram.fst /home/kaldi/tools/openfst/bin/fstcompose L.fst G-3gram.fst LG-3gram.fst 
```

#### 4.1 LdG.fst模型可视化

执行如下命令合并的LdG-Ngram.fst输出为dot格式文件：

```
1 2 3 /home/kaldi/tools/openfst/bin/fstdraw --isymbols=phones.txt --osymbols=words.txt LdG-1gram.fst > LdG-1gram.dot /home/kaldi/tools/openfst/bin/fstdraw --isymbols=phones.txt --osymbols=words.txt LdG-2gram.fst > LdG-2gram.dot /home/kaldi/tools/openfst/bin/fstdraw --isymbols=phones.txt --osymbols=words.txt LdG-3gram.fst > LdG-3gram.dot 
```

修改dot文件中的的size属性：

LdG-1gram.dot：size = “30,48”

LdG-2gram.dot：size = “300,48”

LdG-3gram.dot：size = “300,48”

执行dot命令输出图片：

```
1 2 3 dot -Tjpg LdG-1gram.dot > LdG-1gram.jpg dot -Tjpg LdG-3gram.dot > LdG-2gram.jpg dot -Tjpg LdG-3gram.dot > LdG-3gram.jpg 
```

合并的LdG-Ngram.fst模型图像如下：

![合并的LdG-1gram.fst模型](https://wangkaisine.github.io/https://wangkaisine.github.io//img/in-post/post-wfst-test/LdG-1gram.jpg)

![合并的LdG-2gram.fst模型](https://wangkaisine.github.io/https://wangkaisine.github.io//img/in-post/post-wfst-test/LdG-2gram.jpg)

![合并的LdG-3gram.fst模型](https://wangkaisine.github.io/https://wangkaisine.github.io//img/in-post/post-wfst-test/LdG-3gram.jpg)

> 注：LG-Ngram.fst的可视化同上所述。

### 5. 确定化发音词典与语法模型（det-LG.fst）

#### 5.1 确定化LG.fst模型

执行如下命令将LdG-Ngram模型进行确定化（determinize）

```
1 2 3 /home/kaldi/tools/openfst/bin/fstdeterminize LdG-1gram.fst > det-LdG-1gram.fst /home/kaldi/tools/openfst/bin/fstdeterminize LdG-2gram.fst > det-LdG-2gram.fst /home/kaldi/tools/openfst/bin/fstdeterminize LdG-3gram.fst > det-LdG-3gram.fst 
```

#### 5.2 确定化LG.fst模型可视化

使用上述与LdG.fst模型可视化相同的命令，将fst模型转为dot格式文件，修改对应dot文件中的size大小（相同）。使用dot命令输出图片。

确定化的合并的det-LdG-Ngram.fst模型图像如下：

![确定化的合并的det-LdG-1gram.fst模型](https://www.dazhuanlan.com/img/in-post/post-wfst-test/LdG-1gram.jpg)

![确定化的合并的det-LdG-2gram.fst模型](https://www.dazhuanlan.com/img/in-post/post-wfst-test/LdG-2gram.jpg)

![确定化的合并的det-LdG-3gram.fst模型](https://www.dazhuanlan.com/img/in-post/post-wfst-test/LdG-3gram.jpg)

### 6. 构建上下文模型与发音词典模型、语法模型（CLG.fst）

#### 6.1 合并上下文模型与LG.fst模型

执行如下命令构建CLdG-Ngram.fst模型。

```
1 2 3 /home/kaldi/src/fstbin/fstcomposecontext --context-size=1 --central-position=0  --read-disambig-syms=lang/phones/disambig.int --write-disambig-syms=disambig_ilabels.int disambig_ilabels < ../LdG-1gram.fst > CLdG-1gram.fst /home/kaldi/src/fstbin/fstcomposecontext --context-size=1 --central-position=0  --read-disambig-syms=lang/phones/disambig.int --write-disambig-syms=disambig_ilabels.int disambig_ilabels < ../LdG-2gram.fst > CLdG-2gram.fst /home/kaldi/src/fstbin/fstcomposecontext --context-size=1 --central-position=0  --read-disambig-syms=lang/phones/disambig.int --write-disambig-syms=disambig_ilabels.int disambig_ilabels < ../LdG-3gram.fst > CLdG-3gram.fst 
```

> 注：（1）其中的–context-size=1 –central-position=0选项表示，单音素模型，中间音素位置为0；（2）其中输入文件lang/phones/disambig.int来自上述3.2 lang文件夹与发音词典模型生成中产生的文件，输入文件LdG-Ngram.fst来自于上一步合并的LdG-Ngram.fst模型。

#### 6.2 上下文CLG.fst模型可视化

使用上述与LdG.fst模型可视化相同的命令，将fst模型转为dot格式文件，修改对应dot文件中的size大小（相同）。使用dot命令输出图片。

构建的上下文模型与发音词典模型、语法模型CLdG-Ngram.fst模型图像如下：

![上下文CLdG-1gram.fst模型](https://wangkaisine.github.io//img/in-post/post-wfst-test/CLdG-1gram.jpg)

![上下文CLdG-2gram.fst模型](https://wangkaisine.github.io//img/in-post/post-wfst-test/CLdG-2gram.jpg)

![上下文CLdG-3gram.fst模型](https://wangkaisine.github.io//img/in-post/post-wfst-test/CLdG-3gram.jpg)

### 7. 构建HMM模型（H.fst）

#### 7.1 单音素GNMM-HMM模型与Ha.fst生成

构建HMM模型，需要先初始化定义GMM-HMM结构，确定音素绑定树结构，执行如下命令，生成初始化的GMM-HMM模型（gmm-init.mdl），和音素绑定树（phone.tree）。

```
1 /home/kaldi/src/gmmbin/gmm-init-mono lang/topo 40 gmm-init.mdl phone.tree 
```

执行如下命令，构建Ha.fst模型

```
1 /home/kaldi/src/bin/make-h-transducer --disambig-syms-out=disambig_tid.int disambig_ilabels phone.tree gmm-init.mdl > Ha.fst 
```

> 注：（1）其中gmm-init-mono是生成单音素GMM模型，和上一步操作的–context-size=1 –central-position=0选项对应，gmm-init-model对应生成三音素GMM模型，对应的选项为–context-size=3 –central-position=1；（2）Ha.fst中的a表示没有自环（self-loop）。

#### 7.2 Ha.fst模型可视化

执行如下命令，将Ha.fst模型输出为dot格式文件：

```
1 /home/kaldi/tools/openfst/bin/fstdraw --osymbols=phones.txt Ha.fst > Ha.dot 
```

修改dot文件中的的size属性：

Ha.dot：size = “30,48”

执行dot命令输出图片：

```
1 dot -Tjpg Ha.dot > Ha.jpg 
```

![HMM Ha.fst模型](https://wangkaisine.github.io//img/in-post/post-wfst-test/Ha.jpg)

### 8. 合并HMM模型，上下文模型、发音词典模型、语法模型（HCLG.fst）

#### 8.1 合并Ha.fst模型与CLdG-Ngram.fst模型

执行如下命令，合并Ha.fst与CLdG-Ngram.fst模型：

```
1 2 3 /home/kaldi/src/fstbin/fsttablecompose Ha.fst CLdG-1gram.fst > HaCLdG-1gram.fst /home/kaldi/src/fstbin/fsttablecompose Ha.fst CLdG-2gram.fst > HaCLdG-2gram.fst /home/kaldi/src/fstbin/fsttablecompose Ha.fst CLdG-3gram.fst > HaCLdG-3gram.fst 
```

执行如下命令，确定化HaCLdG.fst模型：

```
1 2 3 /home/kaldi/src/fstbin/fstdeterminizestar HaCLdG-1gram.fst > det-HaCLdG-1gram.fst /home/kaldi/src/fstbin/fstdeterminizestar HaCLdG-2gram.fst > det-HaCLdG-2gram.fst /home/kaldi/src/fstbin/fstdeterminizestar HaCLdG-3gram.fst > det-HaCLdG-3gram.fst 
```

执行如下命令，去除HaCLdG.fst模型中与消歧相关的转移：

```
1 2 3 /home/kaldi/src/fstbin/fstrmsymbols data/disambig_tid.int det-HaCLdG-1gram.fst > det-HaCLG-1gram.fst /home/kaldi/src/fstbin/fstrmsymbols data/disambig_tid.int det-HaCLdG-2gram.fst > det-HaCLG-2gram.fst /home/kaldi/src/fstbin/fstrmsymbols data/disambig_tid.int det-HaCLdG-3gram.fst > det-HaCLG-3gram.fst 
```

执行如下命令，最小化HaCLG.fst模型：

```
1 2 3 /home/kaldi/tools/openfst/bin/fstminimize det-HaCLG-1gram.fst > min-det-HaCLG-1gram.fst /home/kaldi/tools/openfst/bin/fstminimize det-HaCLG-2gram.fst > min-det-HaCLG-2gram.fst /home/kaldi/tools/openfst/bin/fstminimize det-HaCLG-3gram.fst > min-det-HaCLG-3gram.fst 
```

执行如下命令，为HaCLG.fst模型添加自环：

```
1 2 3 /home/kaldi/src/bin/add-self-loops --self-loop-scale=0.1 --reorder=true data/gmm-init.mdl < min-det-HaCLG-1gram.fst > min-det-HCLG-1gram.fst /home/kaldi/src/bin/add-self-loops --self-loop-scale=0.1 --reorder=true data/gmm-init.mdl < min-det-HaCLG-2gram.fst > min-det-HCLG-2gram.fst /home/kaldi/src/bin/add-self-loops --self-loop-scale=0.1 --reorder=true data/gmm-init.mdl < min-det-HaCLG-3gram.fst > min-det-HCLG-3gram.fst 
```

#### 8.2 min-det-HCLG-Ngram.fst模型可视化

执行如下命令合并的LdG-Ngram.fst输出为dot格式文件：

```
1 2 3 /home/kaldi/tools/openfst/bin/fstdraw --osymbols=words.txt min-det-HCLG-1gram.fst > min-det-HCLG-1gram.dot /home/kaldi/tools/openfst/bin/fstdraw --osymbols=words.txt min-det-HCLG-2gram.fst > min-det-HCLG-2gram.dot /home/kaldi/tools/openfst/bin/fstdraw --osymbols=words.txt min-det-HCLG-3gram.fst > min-det-HCLG-3gram.dot 
```

修改dot文件中的的size属性：

min-det-HCLG-1gram.dot：size = “300,48”

min-det-HCLG-2gram.dot：size = “300,48”

min-det-HCLG-3gram.dot：size = “300,48”

执行dot命令输出图片：

```
1 2 3 dot -Tjpg min-det-HCLG-1gram.dot > min-det-HCLG-1gram.jpg dot -Tjpg min-det-HCLG-2gram.dot > min-det-HCLG-2gram.jpg dot -Tjpg min-det-HCLG-3gram.dot > min-det-HCLG-3gram.jpg 
```

最终的min-det-HCLG-Ngram.fst模型图像如下：

![min-det-HCLG-1gram.fst模型](https://wangkaisine.github.io//img/in-post/post-wfst-test/min-det-HCLG-1gram.jpg)

![min-det-HCLG-2gram.fst模型](https://wangkaisine.github.io//img/in-post/post-wfst-test/min-det-HCLG-2gram.jpg)

![min-det-HCLG-3gram.fst模型](https://wangkaisine.github.io//img/in-post/post-wfst-test/min-det-HCLG-3gram.jpg)