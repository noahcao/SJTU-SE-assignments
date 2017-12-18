<template>
  <div class="Papers">
    <h1>{{title}}</h1>
    <div id="intro"><strong>Video Captioning</strong> is a subtask of computer vision, which often mainly researched
       together with <i>Audio Description</i> and <i>Transcription</i>. Cause I don't know much on this exact issue, I
       am looking for some new contributors for this part. Otherwise, I copied the materials on <a href=
        "https://github.com/noahcao/awesome-deep-vision#human-pose-estimation">awesome-deep-vision</a> directly first.
    </div>
    <div class="image">
      <img src="../../static/caption.jpeg" width="50%">
    </div>
    <ul>

      <li> <strong>Long-term Recurrent Convolutional Networks for Visual Recognition and Description</strong>
        <a href="http://arxiv.org/pdf/1411.4389.pdf">[paper]</a>
        <div class="author">
          Jeff Donahue, Lisa Anne Hendricks, Sergio Guadarrama,
          Marcus Rohrbach, Subhashini Venugopalan, Kate Saenko, Trevor Darrell
        </div>
      </li>

      <li> <strong>Translating Videos to Natural Language Using Deep Recurrent Neural Networks</strong>
        <a href="http://arxiv.org/pdf/1412.4729" target="_blank">[paper]</a>
        <div class="author">
          Subhashini Venugopalan, Huijuan Xu, Jeff Donahue, Marcus Rohrbach, Raymond Mooney, Kate Saenko
        </div>
      </li>

      <li> <strong>Joint Modeling Embedding and Translation to Bridge Video and Language</strong>
        <a href="http://arxiv.org/pdf/1505.01861" target="_blank">[paper]</a>
        <div class="author">
          Yingwei Pan, Tao Mei, Ting Yao, Houqiang Li, Yong Rui
        </div>
      </li>

      <li><strong>Sequence to Sequence--Video to Text</strong>
        <a href="http://arxiv.org/pdf/1505.00487" target="_blank">[paper]</a>
        <div class="author">
          Subhashini Venugopalan, Marcus Rohrbach, Jeff Donahue, Raymond Mooney, Trevor Darrell, Kate Saenko
        </div>
      </li>

      <li> <strong>Describing Videos by Exploiting Temporal Structure</strong>
        <a href="https://arxiv.org/pdf/1603.06937.pdf" target="_blank">[paper]</a></li>
      <div class="author">
        Li Yao, Atousa Torabi, Kyunghyun Cho, Nicolas Ballas, Christopher Pal, Hugo Larochelle, Aaron Courville
      </div>
      </li>

      <li><strong>The Long-Short Story of Movie Description</strong>
        <a href="http://arxiv.org/pdf/1506.01698.pdf" target="_blank">[paper]</a>
        <div class="author">
          Anna Rohrbach, Marcus Rohrbach, Bernt Schiele
        </div>
      </li>

      <li><strong>Aligning Books and Movies: Towards Story-like Visual Explanations by Watching Movies and Reading Books</strong>
        <a href="http://arxiv.org/pdf/1506.06724.pdf" target="_blank">[paper]</a>
        <div class="author">
          Yukun Zhu, Ryan Kiros, Richard Zemel, Ruslan Salakhutdinov, Raquel Urtasun, Antonio Torralba, Sanja Fidler
        </div>
      </li>

      <li><strong>Describing Multimedia Content using Attention-based Encoder-Decoder Networks</strong>
        <a href="http://arxiv.org/pdf/1507.01053.pdf" target="_blank">[paper]</a>
        <div class="author">
          Kyunghyun Cho, Aaron Courville, Yoshua Bengio
        </div>
      </li>

      <li><strong>emporal Tessellation for Video Annotation and Summarization</strong>
        <a href="http://arxiv.org/pdf/1506.06724.pdf" target="_blank">[paper]</a>
        <div class="author">
          Dotan Kaufman, Gil Levi, Tal Hassner, Lior Wolf
        </div>
      </li>
    </ul>
  </div>
</template>

<script>
  export default {
    name: 'HelloWorld',
    data () {
      return {
        msg: 'Python Projects',
        app: 'heelo!',
        title: 'Video Captioning'
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
