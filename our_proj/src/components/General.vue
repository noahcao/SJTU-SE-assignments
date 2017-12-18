<template>
  <div class="Papers">
    <h1>{{title}}</h1>
    <div id="intro"><strong>'See and Understand the world automatically'</strong>, thus the main goal of modern computer
      vision, is one of the most critical parts of <i>Artificial Intelligence</i>. To settle this issue, scientists and
       engineers have experienced a long period of research. In recent ten years, methods based on <i>Neural Networks</i>
       has gained good result on almost all sub-task of computer vision. Hence, there is no doubt that these methods have
       become the mainstream already, among which, some classic models must be stressed on.Though some of them may not be able to gain a high score on specific tasks now, research on them still has a high
      directive value.
    </div>

    <div class="image">
      <img src="../../static/general.png" width="65%">
    </div>

    <ul>
      <li> <strong>LeNet</strong>
        <a href="http://www.dengfanxin.cn/wp-content/uploads/2016/03/1998Lecun.pdf">[paper]</a>
        <div class="author">
          Yann LeCun, Leon Bottou, Yoshua Bengio, Patrick Haffner
        </div>
        <div class="caption">
          Gradient-Based Learning Applied to Document Recognition
        </div>
      </li>

      <li> <strong>AlexNet</strong>
        <a href="http://papers.nips.cc/paper/4824-imagenet-classification-with-deep-convolutional-neural-networks.pdf" target="_blank">[paper]</a>
        <div class="author">
          Alex Krizhevsky, Ilya Sutskever, Geoffrey E. Hinton
        </div>
        <div class="caption">
          ImageNet Classification with Deep Convolutional Neural Networks
        </div>
      </li>

      <li> <strong>VGGNet</strong>
        <a href="https://arxiv.org/abs/1409.1556" target="_blank">[paper]</a>
        <div class="author">
          Karen Simonyan, Andrew Zisserman
        </div>
        <div class="caption">
          Very Deep Convolutional Networks for Large-Scale Image Recognition
        </div>
      </li>

      <li> <strong>LSTM</strong>
        <a href="http://www.bioinf.jku.at/publications/older/2604.pdf" target="_blank">[paper]</a>
        <div class="author">
          S Hochreiter, Jürgen Schmidhuber
        </div>
        <div class="caption">
          Long Short-Term Memory
        </div>
      </li>

      <li><strong>ResNet</strong>
        <a href="https://arxiv.org/abs/1512.03385" target="_blank">[paper]</a>
        <div class="author">
          Kaiming He, Xiangyu Zhang, Shaoqing Ren, Jian Sun
        </div>
        <div class="caption">
          Deep Residual Learning for Image Recognition
        </div>
      </li>

      <li> <strong>Stacked Hourglass</strong>
        <a href="https://arxiv.org/pdf/1603.06937.pdf" target="_blank">[paper]</a></li>
      <div class="author">
        Newell, Alejandro, Kaiyu Yang, and Jia Deng
      </div>
      <div class="caption">
        “Stacked hourglass networks for human pose estimation.” arXiv preprint arXiv:1603.06937 (2016).
      </div>
      </li>

      <li><strong>GoogLeNet</strong>
        <a href="https://arxiv.org/abs/1409.4842" target="_blank">[paper]</a>
        <div class="author">
          Christian Szegedy, Wei Liu, Yangqing Jia, Pierre Sermanet <i>et al</i>
        </div>
        <div class="caption">
          Going Deeper with Convolutions
        </div>
        <div class="comment">
          Yangqing Jia, one of authors of this paper, is one of most famous Chinese names in this fields, working
          in Facebook now, who is the main creator of <i>Caffe</i> which is one the most important framework of
          deeplearning.
        </div>
      </li>

      <li><strong>Mask RCNN</strong>
        <a href="https://arxiv.org/pdf/1703.06870.pdf" target="_blank">[paper]</a>
        <div class="author">
          Kaiming He, Georgia Gkioxari, Piotr Dollar, Ross B. Girshick
        </div>
        <div class="caption">
          Mask R-CNN, ICCV 2017, <i>'Best Paper'</i>
        </div>
      </li>
    </ul>
  </div>
</template>

<script>
  export default {
    name: 'general',
    data () {
      return {
        msg: 'Python Projects',
        title: 'Classic Models',
        active: 0
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
  .hide_tab{
    position: absolute;
    bottom: 0;
  }
</style>
