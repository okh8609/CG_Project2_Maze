# NTUST Computer Graphics Project2 Maze
[LINK](https://hackmd.io/@fqiObQWCSUWMlLKBbLIqXg/SyTCC1qYS)

![](https://i.imgur.com/C46h9TI.gif)

<font color=#EE0000>請勿將專案存放在含有中文的路徑下, 讀取迷宮檔時將有可能造成錯誤</font>

## ※ 題目摘錄與提示：
* 地板在z = -1，天花板在z = 1
* 鏡頭的z始終為0
* 建議你實作一個 Frustum Class，該Class存儲有關查看的視錐的信息
* 把 `Mini_Map()` 中，畫視錐的線段加長(`float len = 10;`)，比較好debug
* `QLine`很好用，有提供 算交點、算角度的function
* `QVector` 和 `QMatrix` 有需要也可以拿來用
* 注意，原本助教提供的畫左半邊的code，即使房間是長方形，也會拉乘正方形
    有時候看起來會怪怪的，建議把它改成正常比例。
    ![](https://i.imgur.com/0J7cFDw.png)
    
    ![](https://i.imgur.com/LBDc0Ms.png)




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

### 找一個檔案開起來自己畫畫看就知道了
![](https://i.imgur.com/gPrNkQL.jpg =500x370)


</br></br>
## ※ 算法說明： (OK的版本)

### **1. 遞迴找哪個Cell、哪道牆(edge)該畫**

### **> Cell and Portal visibility algorithm**
![](https://i.imgur.com/QKJPfzW.png)
* 必須實作
    *  The function **Draw_Cell(C, F)** is initially called with the cell containing the viewer, and the full view frustum. 
    *  The **neighbor(C,E)** function returns the cell's neighbor across the edge.
* 注意：
    * 剪裁時區別四種情況
        左切邊做一次，右切邊再做一次
        * 都在視錐內
            * 畫
        * 往視錐邊界射出去
            * 終點設為與切邊的交點
        * 從視錐邊界穿近來
            * 起點設為與切邊的交點
        * 都在視錐外
            * 就直接忽略

</br></br>

**<font color=#0000FF>
然後是各種座標轉換
World space -> Camera Space -> Perspective Projection
</font>**


### **2. World space -> Camera Space**
<font color=#00AAAA>相關說明請搭配下圖觀看。</font>

這個專案可以直接用 **2D的方式** 下去思考。程式左半邊迷宮的所有線段已經是處在 **World space** 中。我們透過 **線性代數** 教過的 **基底的轉換** ，把線段的 **起點** 與 **終點** 轉到 **Camera Space** 中。

1. 把 <b>物體(m,n)</b> 與 <b>相機原點(a,b)</b> 平移到 <b>歐式座標的原點</b> 。
2. 試圖找出 <b>x'</b> 與 <b>y'</b> 係數，得以用 <b>D向量(相機方向)</b> 與 <b>R向量(相機右方)</b> 線性組合出物體的所在位置。
3. 經過下圖中式子的推導，得知：只要乘一個 <b>反矩陣</b> 就OK了。

![](https://i.imgur.com/WwOEKtO.jpg)


</br></br>
### **3. Camera Space -> Perspective Projection**
<font color=#00AAAA>相關說明請搭配下圖觀看。</font>

接著，想先知道 ***相機* 與 *[-1,1]屏幕* 的距離( d )**
透過 fov 可以輕易地得知 (下圖中 左半邊 橘色筆所示)

有了 **d** 之後，就可以用相似三角形，取得物體投影到屏幕上的交點座標。
(如下圖 中間偏下方的那個三角形所示。)

再來，我們要把空間轉到 3D 的狀態了
讓物體的點座標在平面上的**高度** ， **向上1單位(x', y', 1)** 與 **向下1單位(x', y', -1)**
然後用 **點的距離概念** 與 **相似三角形** ，可以求得投影在屏幕上的點座標高度 **k1** **k2**

![](https://i.imgur.com/i7vkDRR.jpg)

</br></br>

