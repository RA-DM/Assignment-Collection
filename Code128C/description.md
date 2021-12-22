<h1> code128C条形码的实现</h1>

##### 1. 目的：不导入barby等外部条形码库，用ruby实现code128C码的生成

##### 2. 运行环境：

######		-OS:

windows 10

###### -**依赖:**

ruby -3.0.2p107

ruby2d -0.11.0



##### 3. code128C原理简要说明：

> CODE128码是广泛应用在企业内部管理、生产流程、物流控制系统方面的条码码制，由于其优良的特性在管理信息系统的设计中被广泛使用。其可以对全部128个[ASCII](https://zh.wikipedia.org/wiki/ASCII)字符（包括数字、大小写字母、符号和控制符）进行编码。其有三种不同的版本：A、B、C。
>
> * CODE128A:标准数字和大写字母,控制符,特殊字符
>
> * CODE128B:标准数字和大写字母,小写字母,特殊字符
>
> * CODE128C:[00]-[99]的数字对集合,共100个
>
> ​																																				-引用自百度百科和维基百科

code128编码将每个ASCII字符和若干控制位（如开始位）映射成数量不同的黑白条块（即黑白线）；字符映射条块和相应的校检位、开始结束条块组成了code128条形码，下图是三个code128版本的部分映射图：

> <img src="C:\Users\he583\AppData\Roaming\Typora\typora-user-images\image-20211025195705263.png" alt="image-20211025195705263" style="zoom:50%;" />
>
> ​						-引用自个人博客：https://www.chenxublog.com/2018/09/22/wechat-alipay-barcode-code128.html



本程序采用code128C版本来实现偶数位/奇数位数字的表示。对于code128C码，其一个条块组合对应一组数字（如“03”），编码规则如下：（为简便表示，将b表示黑条，s表示白条）

> **最终条码=总开始位（映射的黑白条块，以下省略） + 后面所有的数据按顺序拼接 + 校验位 + 结束位；**
>
> ​	其中校检码 id=开始位“startC”在C版本映射中的id + 第i个数值的ID*i)%103

以数字串“0102”为例，其条码表示为:

​		bbsbssbbbss（C版本开始位）+ bbssbbsbbss(“01”)+ bbssbbssbbs(“02”) + bssbbsssbss（校检码）+ bbsssbbbsbsbb（停止位）

特别的，对于奇数位的数字串来说，我们将最后一位分割出来，单独采用其他版本的code128码来处理该值，本程序中采用code128A版本。即之前条块后加上 标识符‘codeA’ 的对应条块（id: 101），并将最后一位数字在A版本中的对应条块加上，同时将该数字在A版本中的id加入校检码计算。



##### 4. 程序结构功能说明及部分源码展示：

1. 文件包含两个文件：code128C_mapping.txt和主程序，前者保存对应映射关系，后者生成条码;

   * 除去预处理和一些哈希集合定义，主程序包含generateCode128c和generatePicture两函数，前者生成数字串对应的code（bsbs...）, 后者根据前者调用ruby2d画图。

2. 部分代码：

   ```ruby
   def generateCode128c(s)
       if s==""
           puts "Error: input is null"
           return ""
       end
       result=$codeC_map[105]
       checkBit=105
       numbers = []
   
       #even length without code128A
       if s.length%2==0
           i=1
           s.chars.each_slice(2) do |slice|
               index=Integer(slice.join)
               checkBit+=index*i
               result+=$codeC_map[index]
               i+=1
           end
           result+=$codeC_map[checkBit%103]+$codeC_map[106]
           return result
       #omit remained part......
   ```

   
