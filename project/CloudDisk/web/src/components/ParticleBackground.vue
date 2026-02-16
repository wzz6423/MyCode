<template>
  <div class="mesh-bg">
    <div class="mesh-layer"></div>
    <div class="noise"></div>
  </div>
</template>

<style scoped>
.mesh-bg {
  position: fixed;
  inset: 0;
  z-index: 0;
  overflow: hidden;
  background-color: #f9fafb;
}

.mesh-layer {
  position: absolute;
  inset: -20%;
  width: 140%;
  height: 140%;
  background:
    radial-gradient(ellipse 60% 50% at 10% 20%, rgba(100, 140, 255, 0.35) 0%, transparent 60%),
    radial-gradient(ellipse 50% 60% at 85% 15%, rgba(140, 100, 240, 0.28) 0%, transparent 55%),
    radial-gradient(ellipse 55% 45% at 70% 80%, rgba(53, 107, 251, 0.3) 0%, transparent 55%),
    radial-gradient(ellipse 45% 55% at 25% 75%, rgba(160, 120, 255, 0.22) 0%, transparent 50%),
    radial-gradient(ellipse 50% 50% at 55% 45%, rgba(56, 189, 248, 0.25) 0%, transparent 55%);
  animation: meshShift 40s ease-in-out infinite alternate;
}

.noise {
  position: absolute;
  inset: 0;
  background-image: url("data:image/svg+xml,%3Csvg viewBox='0 0 256 256' xmlns='http://www.w3.org/2000/svg'%3E%3Cfilter id='n'%3E%3CfeTurbulence type='fractalNoise' baseFrequency='0.85' numOctaves='4' stitchTiles='stitch'/%3E%3C/filter%3E%3Crect width='100%25' height='100%25' filter='url(%23n)' opacity='0.025'/%3E%3C/svg%3E");
  background-repeat: repeat;
  background-size: 256px 256px;
  pointer-events: none;
}

@keyframes meshShift {
  0% {
    transform: translate(0, 0) rotate(0deg);
  }
  100% {
    transform: translate(-8%, -5%) rotate(3deg);
  }
}
</style>
