# B+树

索引 Key 和数据 T 都有默认构造函数和拷贝构造函数（为了偷懒）

叶子节点相较普通节点少了指出的指针，但是多了值的存储

## 数据存储：

树结构和索引分开存储

树结构（tree）

数据（data）

节点回收站（tree_recycle）

数据回收站（data_recycle）

每个节点需要储存：父亲指针（parent），左右兄弟指针（left/right），孩子指针（child[]），索引（keys[]），存储索引数量（siz），节点在外存中的地址（addr），是否是非叶子节点（is_node）

## 迭代器

迭代器中存储节点信息和数据编号

### operator++ , operator-- 

因为记录了左右兄弟，因此可以方便维护

## 成员函数

### insert(Key, T)

插入键值为 Key 数据为 T 的元素，支持重复插入

### erase(Key, T)

删除键值为 Key 数据为 T 的元素，如果该元素不存在，则 throw erase_fail

### find(Key)

寻找键值为 Key 的元素的数据 T ，如果有多个，返回第一个，如果找不到，则

### lower_bound(Key) 

寻找键值等于 Key 的第一个元素，返回一个迭代器

### upper_bound(Key)

寻找键值大于 Key 的第一个元素，返回一个迭代器
