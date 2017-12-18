<template>
  <div class="Papers">
    <h1>{{title}}</h1>
    <div id="intro"><strong>Segmentation</strong> mainly focus on the issue of <i>'find the outline of object skeleton'.</i>
      Cause the <i>Object Detection</i> has gained very reliable result on a lot of categories and under different situations,
       <i>Segmentation</i> has been paied more and more attention in recent years. At the begining, related work mainly focuses
       on <i>Semantic Segmentation</i> which just gives a uniform mask of a region among which a sole kind of objects is
       detected. Moreover, recent works are more and more pays attention of <i>Instance Segmentation</i> which rises the accuracy
       of mask note to a 'instance level'. Note that <a href="http://cocodataset.org">COCO Challenges</a>, held by Microsoft,
        is one of most highlighted competition on both <i>Segmentation</i> and <i>Pose Estimation</i> issues.
    </div>

    <div class="image">
      <img src="../../static/seg.png" width="60%">
    </div>
    <div style="padding: 20px 50px 20px 50px;background: white;font-size: 120%">The leaderboard of <strong>
      COCO2017 Competition on Segmentation</strong> shown below, which gives the most state-of-art works on segmentation
      currently:</div>

    <div class="image">
      <img src="../../static/cocoseg.png" width="40%">
    </div>

    <h2>Semantic Segmentation</h2>
    <ul>
      <li><strong>Deep Parsing Network(DPN)</strong>
        <a href="https://arxiv.org/pdf/1509.02634.pdf" target="_blank">[paper]</a>
        <div class="author">
          Ziwei Liu, Xiaoxiao Li, Ping Luo, Chen Change Loy, Xiaoou Tang
        </div>
        <div class="caption">
          Semantic Image Segmentation via Deep Parsing Network, arXiv:1509.02634 / ICCV 2015
        </div>
      </li>

      <li><strong>BoxSup</strong>
        <a href="https://arxiv.org/pdf/1503.01640" target="_blank">[paper]</a>
      <div class="author">
        Jifeng Dai, Kaiming He, Jian Sun
      </div>
      <div class="caption">BoxSup: Exploiting Bounding Boxes to Supervise Convolutional
        Networks for Semantic Segmentation, arXiv:1503.01640. (6th ranked in VOC2012)
      </div>
      </li>
      <li><strong>Conditional Random Fields as Recurrent Neural Networks</strong>
        <a href="http://arxiv.org/pdf/1502.03240" target="_blank">[paper]</a>
      <div class="author">
        Shuai Zheng, Sadeep Jayasumana, Bernardino Romera-Paredes, Vibhav Vineet, Zhizhong Su, Dalong Du, Chang Huang, Philip H. S. Torr
      </div>
        <div class="caption">
          Conditional Random Fields as Recurrent Neural Networks, arXiv:1502.03240. (8th ranked in VOC2012)
        </div>
      </li>

      <li><strong>DeepLab</strong>
        <a href="https://arxiv.org/pdf/1502.02734" target="_blank">[paper]</a>
        <div class="author">
          Liang-Chieh Chen, George Papandreou, Kevin Murphy, Alan L. Yuille
        </div>
        <div class="caption">
          Weakly-and semi-supervised learning of a DCNN for semantic image segmentation, arXiv:1502.02734
        </div>
      </li>

      <li><strong>FCN for Semantic Segmentation</strong>
        <a href="http://www.cv-foundation.org/openaccess/content_cvpr_2015/papers/Long_Fully_Convolutional_Networks_2015_CVPR_paper.pdf"
           target="_blank">[paper]</a>
        <div class="author">
          Jonathan Long, Evan Shelhamer, Trevor Darrell
        </div>
        <div class="caption">
          Fully Convolutional Networks for Semantic Segmentation, CVPR, 2015.
        </div>
      </li>

      <li><strong>Bayesian SegNet</strong>
        <a href="http://arxiv.org/abs/1511.00561">[paper]</a>
        <div class="author">
          Vijay Badrinarayanan, Alex Kendall and Roberto Cipolla
        </div>
        <div class="caption">
          "SegNet: A Deep Convolutional Encoder-Decoder Architecture for Image Segmentation." arXiv preprint arXiv:1511.00561, 2015
        </div>
      </li>
    </ul>


    <h2>Instance Segmentation</h2>
    <ul>
    <li><strong>DeepMask</strong>
      <a href="https://arxiv.org/abs/1506.06204" target="_blank">[paper]</a>
      <a href="https://github.com/facebookresearch/deepmask">[code]</a>
      <div class="author">
        Pedro O. Pinheiro, Ronan Collobert, Piotr Dollar
      </div>
      <div class="caption">
        Learning to Segment Object Candidates, arXiv:1506.06204
      </div>
    </li>

    <li><strong>SharpMask</strong>
      <a href="https://arxiv.org/abs/1603.08695" target="_blank">[paper]</a>
      <a href="https://github.com/facebookresearch/deepmask">[code]</a>
      <div class="author">
        Pedro O. Pinheiro, Tsung-Yi Lin, Ronan Collobert, Piotr Dollar
      </div>
      <div class="caption">
        Learning to Refine Object Segments, arXiv:1603.08695
      </div>
    </li>

    <li><strong>InstanceFCN</strong>
      <a href="https://arxiv.org/abs/1603.08678" target="_blank">[paper]</a>
      <div class="author">
        Jifeng Dai, Kaiming He, Yi Li, Shaoqing Ren, Jian Sun
      </div>
      <div class="caption">
        Instance-sensitive Fully Convolutional Networks, ECCV 2016
      </div>
    </li>

    <li><strong>R-FCN</strong>
      <a href="https://arxiv.org/abs/1605.06409" target="_blank">[paper]</a>
      <div class="author">
        Jifeng Dai, Yi Li, Kaiming He, Jian Sun
      </div>
      <div class="caption">
        R-FCN:Object Detection via Region-based Fully Convolutional Networks, NIPS 2016
      </div>
    </li>
    </ul>
  </div>
</template>

<script>
  export default {
    name: 'Seg',
    data () {
      return {
        title: 'Segmentation'
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
    padding-left: 50px;
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
</style>
