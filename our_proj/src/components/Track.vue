<template>
  <div class="Papers">
    <h1>{{title}}</h1>
    <div id="intro"><strong>'Object Tracking'</strong> is to have a real-time tracking on detected objects. In recent
       years, object tracking has been more and more applied in industrial fields. And different from many other branches
       of computer vision, object tracking is adopted and researched on video data at a very early timepoint.
    </div>
    <div class="image">
      <img src="../../static/track.png" width="60%">
    </div>
    <ul>
      <li> <strong>Online Tracking by Learning Discriminative Saliency Map with Convolutional Neural Network</strong>
        <a href="http://arxiv.org/pdf/1502.06796">[paper]</a>
        <div class="author">
          Seunghoon Hong, Tackgeun You, Suha Kwak, Bohyung Han
        </div>
      </li>

      <li> <strong>DeepTrack</strong>
        <a href="http://www.bmva.org/bmvc/2014/files/paper028.pdf" target="_blank">[paper]</a>
        <div class="author">
          Hanxi Li, Yi Li and Fatih Porikli
        </div>
        <div class="caption">
          DeepTrack: Learning Discriminative Feature Representations by Convolutional Neural Networks for Visual Tracking, BMVC, 2014
        </div>
      </li>

      <li> <strong>Learning a Deep Compact Image Representation for Visual Tracking</strong>
        <a href="http://winsty.net/papers/dlt.pdf" target="_blank">[paper]</a>
        <div class="author">
          N Wang, DY Yeung, NIPS, 2013
        </div>
      </li>

      <li><strong>Visual Tracking with fully Convolutional Networks</strong>
        <a href="http://202.118.75.4/lu/Paper/ICCV2015/iccv15_lijun.pdf" target="_blank">[paper]</a>
        <a href="https://github.com/scott89/FCNT">[code]</a>
        <div class="author">
          Lijun Wang, Wanli Ouyang, Xiaogang Wang, and Huchuan Lu, ICCV, 2015
        </div>
      </li>

      <li> <strong>Hierarchical Convolutional Features for Visual Tracking</strong>
        <a href="http://www.cv-foundation.org/openaccess/content_iccv_2015/papers/Ma_Hierarchical_Convolutional_
        Features_ICCV_2015_paper.pdf" target="_blank">[paper]</a>
        <a href="https://github.com/jbhuang0604/CF2">[code]</a>
      <div class="author">
        Chao Ma, Jia-Bin Huang, Xiaokang Yang and Ming-Hsuan Yang, ICCV, 2015
      </div>
      </li>

      <li><strong>Learning Multi-Domain Convolutional Neural Networks for Visual Tracking</strong>
        <a href="http://arxiv.org/pdf/1510.07945.pdf" target="_blank">[paper]</a>
        <div class="author">
          Hyeonseob Namand Bohyung Han
        </div>
      </li>

    </ul>
  </div>
</template>

<script>
  export default {
    name: 'tracking',
    data () {
      return {
        title: 'Tracking'
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
</style>
