/**
 * Created by noahcao on 2017/11/19.
 */
import Pose from '@/components/Pose';
import General from '@/components/General';
import Track from '@/components/Track';
import Video from '@/components/Video';
import Det from '@/components/Det';
import Seg from '@/components/Seg';
import Vue from 'vue'

describe('Pose.vue',() => {
  it('shows the content', () => {
    // test
    const pose = new Vue(Pose).$mount();
    expect(pose.title).toBe('Pose Estimation');
  })
})

describe('General.vue',() => {
  it('shows the content', () => {
    // test
    const general = new Vue(General).$mount();
    expect(general.title).toBe('Classic Models');
  })
})

describe('Track.vue',() => {
  it('test the tile', () => {
    // test
    const track = new Vue(Track).$mount();
    expect(track.title).toBe('Tracking');
  })
})

describe('Video.vue',() => {
  it('shows the content', () => {
    // test
    const video = new Vue(Video).$mount();
    expect(video.title).toBe('Video Captioning');
  })
})

describe('Det.vue',() => {
  it('shows the content', () => {
    // test
    const det = new Vue(Det).$mount();
    expect(det.title).toBe('Object Detection');
  })
})

describe('Seg.vue',() => {
  it('shows the content', () => {
    // test
    const seg = new Vue(Seg).$mount();
    expect(seg.title).toBe('Segmentation');
  })
})
