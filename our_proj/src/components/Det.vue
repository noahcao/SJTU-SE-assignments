<template>
  <div class="Papers">
    <h1>{{title}}</h1>
    <div id="intro"><strong>Object Detection</strong> is a fundamental issue in the field of Computer Vision.
      Cause as the popular insight on computer vision field, <i>"See object automatically"</i> and
      <i>"Recognize object automatically"</i> are always the first step to handle.</i> Historically, object
      detection is, for a long time, researched combined with <i>Graphics</i> and <i>Possibilistic Methods</i>
      . However, currently state-of-art works are mainly based on the <i>Machine Learning</i>. Note that the most
      famous global competition in computer vision, <a href="http://www.image-net.org">ImageNet</a> also sets
      <i>Object Detection</i> as the first-place issue.</div>
    <div class="image">
      <img  src="../../static/det1.png" width="60%">
    </div>
    <ul>
      <li> <strong>PVANET</strong>
        <a href="https://arxiv.org/pdf/1608.08021.pdf">[paper]</a>
        <a href="https://github.com/sanghoon/pva-faster-rcnn">[code]</a>
        <div class="author">
          Kye-Hyeon Kim, Sanghoon Hong, Byungseok Roh, Yeongjae Cheon, Minje Park
        </div>
        <div class="caption">
          PVANET: Deep but Lightweight Neural Networks for Real-time Object Detection, arXiv:1608.08021
        </div>
      </li>

      <li> <strong>R-CNN</strong>
        <a href="http://202.120.0.1/cache/8/03/www.cv-foundation.org/527691
        4452b7b0a3af54b20d58acfaa6/Girshick_Rich_Feature_Hierarchies_2014_CVPR_paper.pdf" target="_blank">[paper]</a>
        <div class="author">
          Ross Girshick, Jeff Donahue, Trevor Darrell, Jitendra Malik
        </div>
        <div class="caption">
          Rich feature hierarchies for accurate object detection and semantic segmentation, CVPR, 2014
        </div>
      </li>

      <li> <strong>Fast R-CNN</strong>
        <a href="https://arxiv.org/pdf/1504.08083" target="_blank">[paper]</a>
        <div class="author">
          Ross Girshick, Microsoft Research
        </div>
        <div class="caption">
          Fast R-CNN, arXiv:1504.080832v2
        </div>
        <div class="comment">
            This work is a beginning of a series of great works, among which many ideas have become very fundamental and classic
            models in many branches of computer vision.
        </div>
      </li>

      <li> <strong>Faster R-CNN</strong>
        <a href="https://arxiv.org/pdf/1506.01497.pdf" target="_blank">[paper]</a></li>
        <div class="author">
          Shaoqing Ren, Kaiming He, Ross Girshick, Jian Sun
        </div>
        <div class="caption">
          Faster R-CNN: Towards Real-Time Object Detection with Region Proposal Networks, arXiv:1506.01497.
        </div>
        <div class="comment">
          This paper has an All-Star author list, among which, the first author <a href="https://github.com/shaoqingren">
          Shaoqing Ren</a> is my senior at high school, and <a href="http://kaiminghe.com">Kaiming He</a> is without doubt
          one of superstar in cv fields, who just captured <i>"double best papers'</i> at <i>ICCV2017</i> with very awesome works.
        </a>
        </div>
      </li>

      <li><strong>Deep Residual Network</strong>
        <a href="https://arxiv.org/abs/1512.03385" target="_blank">[paper]</a>
        <div class="author">
          Kaiming He, Xiangyu Zhang, Shaoqing Ren, Jian Sun
        </div>
        <div class="caption">
          Deep Residual Learning for Image Recognition
        </div>
      </li>

      <li><strong>R-FCNN</strong>
        <a href="https://arxiv.org/abs/1605.06409" target="_blank">[paper]</a>
        <a href="https://github.com/daijifeng001/R-FCN">[code]</a>
        <div class="author">
          Jifeng Dai, Yi Li, Kaiming He, Jian Sun
        </div>
        <div class="caption">
          R-FCN: Object Detection via Region-based Fully Convolutional Networks
        </div>
        <div class="comment">
          <a href="jifengdai.org">Jideng Dai</a>, as a new star in MSRA, has put forward many amazing works.
        </div>
      </li>
    </ul>
    <div id="demo">
      <h1>A Demo</h1>
      Enter the URL to an image that includes a face or faces, then click the <strong>Analyze face</strong> button.
      <br><br>
      Image to analyze: <input type="text" name="inputImage" id="inputImage"
                               value="http://www.se.sjtu.edu.cn/upload/201611/07/201611072035261783.jpg" />
      <button onclick="processImage()">Analyze face</button>
      <br><br>
      <div id="wrapper">
        <div id="jsonOutput" align="center">
          <strong>Response:</strong>
          <br><br>
          <textarea id="responseTextArea" class="UIInput"></textarea>
        </div>
        <div id="imageDiv">
          <strong>Source image:</strong>
          <br><br>
          <img id="sourceImage" width="100%" />
        </div>
      </div>
      <p>
      </p>
    </div>
  </div>
</template>

<script>
export default {
  name: 'HelloWorld',
  data () {
    return {
      msg: 'Python Projects',
      title: 'Object Detection'
    }
  }
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
  .Papers{
    background: white;
  }
  #intro{
    font-size: 120%;
    padding: 20px 50px 20px 50px;
    background: white;
  }
  .author{
    padding: 5px 20px 5px 20px;
    font-style: italic;
    margin-top: 10px;
  }
  .caption{
    padding: 5px 20px 5px 20px;
  }
  .comment{
    padding: 5px 20px 5px 20px;
    font-style: normal;
    background: white;
  }
  div {
    text-align: left;
  }
  h1, h2 {
    font-size: 200%;
    padding-top: 20px;
  }
  ul {
    padding: 20px 100px 20px 100px;
  }
  li {
    padding: 5px;
    font-size: 120%;
  }
  a {
    color: #000000;
    font-size: larger;
  }
  .image{
    display: flex;
    justify-content: center;
    align-items: center;
    background: white;
  }
  image{
    width: auto;
    height: auto;
    max-width: 100%;
    max-height: 100%;
  }
  #demo{
    align-self: center;
    text-align: center;
    background: white;
    border: groove;
    margin-left: 10%;
    width: 80%;
    height: 1000px;
  }
  #wrapper{
    text-align: center;
    align: center;
    display: flex;
    background: white;
  }
  #imageDiv{
    background: none;
    width: 40%;
    max-height: 80%;
    float: left;
    margin-right: 10%;
    margin-left: 10%;
  }
  #jsonOutput{
    background: white;
    margin-right: 10%;
    margin-left: 10%;
    width: 40%;
  }
  .UIInput{
    height: 80%;
    width: 100%;
    align: center;
  }
</style>
