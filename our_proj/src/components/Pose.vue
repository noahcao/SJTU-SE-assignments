<template>
  <div class="Papers">
    <h1>{{title}}</h1>
    <div id="intro"><strong>Pose Estimation</strong> is the issue focusing on estimating the keypoints of a human
     object such as his wrist, head, foot and so on. Based on this, the posture and moving tend of a human could
    be estimated, which has a lot of application in industrial fields and also forms a important branch of computer
     vision. Note that <i>COCO Challenges Keypoint Track</i> is one of most important competitions of this sub-category
      of computer vision.
    </div>

    <div class="image">
      <img src="../../static/kp.png" width="50%">
    </div>
    <div style="padding: 20px 50px 20px 50px;background: white;font-size: 120%">The leaderboard of <strong>
      COCO2017 Competition on Keypoint</strong> shown below, which gives the most state-of-art works on pose estimation
      currently. Specifically, <i>Shanghai Jiaotong University</i> joined this competition also, and ranks 6th, toughly
      behind the <i>Fackbook AI Research</i>, I also took part in this work.
    </div>

    <div class="image">
      <img src="../../static/cocokp.png" width="40%">
    </div>
    <ul>
      <li><strong>Joint training of a convolutional network and a graphical model for human pose estimation</strong>
        <a href="https://arxiv.org/pdf/1406.2984.pdf" target="_blank">[paper]</a>
        <div class="author">
          Jonathan J. Tompson, Arjun Jain, Yann LeCun, Christoph Bregler, NIPS, 2014
        </div>
      </li>

      <li><strong>Flowing convnets for human pose estimation in videos</strong>
        <a href="https://arxiv.org/pdf/1506.02897.pdf" target="_blank">[paper]</a>
        <div class="author">
          Tomas Pfister, James Charles, and Andrew Zisserman, ICCV, 2015.
        </div>
      </li>

      <li><strong>Realtime Multi-Person 2D Pose Estimation using Part Affinity Fields</strong>
        <a href="https://arxiv.org/pdf/1611.08050.pdf" target="_blank">[paper]</a>
        <div class="author">
          Zhe Cao, Tomas Simon, Shih-En Wei, and Yaser Sheikh, CVPR, 2017
        </div>
      </li>

      <li> <strong>Convolutional pose machines</strong>
        <a href="https://arxiv.org/pdf/1602.00134.pdf">[paper]</a>
        <div class="author">
          Shih-En Wei, Varun Ramakrishna, Takeo Kanade, and Yaser Sheikh
        </div>
      </li>

      <li> <strong>Hourglass</strong>
        <a href="https://arxiv.org/pdf/1603.06937.pdf" target="_blank">[paper]</a>
        <div class="author">
          Alejandro Newell, Kaiyu Yang, and Jia Deng
        </div>
        <div class="caption">
          Stacked hourglass networks for human pose estimation, ECCV 2016
        </div>
      </li>

      <li> <strong>Flowing convnets for human pose estimation in videos</strong>
        <a href="https://arxiv.org/pdf/1506.02897.pdf" target="_blank">[paper]</a>
        <div class="author">
          Tomas Pfister, James Charles, and Andrew Zisserman
        </div>
      </li>

      <li> <strong>RMPE</strong>
        <a href="https://arxiv.org/abs/1612.00137" target="_blank">[paper]</a></li>
      <div class="author">
        Hao-Shu Fang, Shuqin Xie, Yu-Wing Tai, Cewu Lu
      </div>
      <div class="caption">
        RMPE: Regional Multi-person Pose Estimation, ICCV 2017
      </div>
      <div class="comment">
        The work I took part in for <i>COCO2017 Challenge for Keypoint Track</i> is partly based on model raised in this
        paper, and Hao-Shu Fang and Shuqin Xie are both my good friend and seniors.
      </div>
      </li>

      <li><strong>Learning Knowledge-guided Pose Grammar Machine for 3D Human Pose Estimation</strong>
        <a href="https://arxiv.org/pdf/1710.06513" target="_blank">[paper]</a>
        <div class="author">
          Haoshu Fang, Yuanlu Xu, Wenguan Wang, Xiaobai Liu, Song-Chun Zhu
        </div>
        <div class="caption">
          Learning Knowledge-guided Pose Grammar Machine for 3D Human Pose Estimation, AAAI 2017
        </div>
      </li>

      <li><strong>Realtime Multi-Person 2D Pose Estimation using Part Affinity Fields</strong>
        <a href="https://arxiv.org/pdf/1611.08050.pdf" target="_blank">[paper]</a>
        <div class="author">
          Zhe Cao, Tomas Simon, Shih-En Wei, and Yaser Sheikh, CVPR, 2017
        </div>
      </li>
    </ul>
  </div>

</template>

<script>
  export default {
    name: 'pose',
    data () {
      return {
        title: 'Pose Estimation'
      }
    }
  }
</script>




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
