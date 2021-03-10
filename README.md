<h1>gthread</h1>
<p> A (green) thread library running in user-space (non-kernel mode) using signal interrupts to swap between contexts. Uses POSIX's <code>ucontext</code> library to swap between contexts.</p>
<h2>Build & Use</h2>
<p>Instructions for building and using the library.</p>
<h3>Requirements</h3>
<p>The requirements are listed below:</p>
<ul>
  <li><code>ucontext</code> - POSIX-library for context-management</li>
</ul>
<h3>Build</h3>  
<ol>
  <li>Use CMake to configure and generate build environment.</li>
  <li>Build the library!</li>
</ol>
<h3> Usage </h3>
<ol>
  <li>Include <code>green.h</code></li>
  <li>Use the library as you would use pthreads!</li> 
  <li>Link to <code>gthread.lib</code> when compiling</li>
</ol>
