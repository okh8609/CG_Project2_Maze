# NTUST Computer Graphics Project2 Maze

<font color=#CC0000>請勿將專案存放在含有中文的路徑下, 讀取迷宮檔時將有可能造成錯誤</font>

## ※ 題目摘錄：
* 地板在z = -1，天花板在z = 1
* 鏡頭的z始終為0
* 建議你實作一個 Frustum Class，該Class存儲有關查看的視錐的信息


</br>

## ※ 輸入說明：
* **vertex 數目**
* 各點 (x,y)
* **edge 數目**
* 各邊
    * 起點編號
    * 終點編號
    * the **index of the cell** to the **left**
    * the **index of the cell** to the **right**
        * 左右 係為 由 **始點** 向 **終點** 看過去之左右
        * 若無 則為 -1
            * <font color=#008800>你可能會覺得困惑，哪來的index of the cell??
                其實它已經跟接下來要輸入的cell資訊互相搭配好了</font>
    * edge不透明，則為1；edge透明，則為0 
    * RGB顏色
* **cell數目**
* 逆時針順序 給出圍繞各cell的edge順序 <font color=#009900>→ ↑ ← ↓</font>
* **相機資訊**
    * 位置(x, y, z)
    * 看向角度 →
    * FOV

</br>

## ※ 算法說明：

### **1. 遞迴找哪個Cell、哪道牆(edge)該畫 ?**

### **> Cell and Portal visibility algorithm**
![](https://i.imgur.com/QKJPfzW.png)
* 必須實作
    *  The function **Draw_Cell(C, F)** is initially called with the cell containing the viewer, and the full view frustum. 
    *  The **neighbor(C,E)** function returns the cell's neighbor across the edge.

</br></br>

**<font color=#0000FF>
然後是各種座標轉換
World space -> Camera Space -> Perspective Projection
</font>**


### **2. World space -> Camera Space**
![](https://i.imgur.com/5FiGLQ4.png =500x110)

![](https://i.imgur.com/c6TSieP.png =300x90)

</br></br>
### **3. Camera Space -> Perspective Projection**
![](https://i.imgur.com/x6BXxoj.png =350x180)

![](https://i.imgur.com/t5zTH3y.png =250x100)

![](https://i.imgur.com/5P5UaSo.png =450x120)

<font color=#FF0000>d 要設啥???</font>

</br></br>
### **4. 壓縮到 (-1,-1) 到 (1,1) 之間**
????
????







<font color=#0000FF></font>
<font color=#FF0000></font>
<font color=#00FF00></font>