(function(){
  const $ = (sel, root=document) => root.querySelector(sel);
  const $$ = (sel, root=document) => Array.from(root.querySelectorAll(sel));

  // NAV LINKS + ACTIVE HIGHLIGHT
  const navLinks = $$('.nav-link');
  navLinks.forEach(link => {
    link.addEventListener('click', (e) => {
      e.preventDefault();
      const id = link.getAttribute('data-section-link');
      const section = document.getElementById(id);
      if(!section) return;
      section.scrollIntoView({behavior:'smooth', block:'start'});
      setTimeout(() => section.focus(), 300);
    });
  });

  const sections = $$('.section');
  const io = new IntersectionObserver((entries) => {
    entries.forEach(entry => {
      if(entry.isIntersecting){
        const id = entry.target.getAttribute('id');
        navLinks.forEach(a => a.classList.toggle('active', a.getAttribute('data-section-link') === id));
      }
    });
  }, {root: null, rootMargin: '-50% 0px -45% 0px', threshold: 0.01});
  sections.forEach(s => io.observe(s));

  // THEME
  const THEME_KEY = 'ride.theme';
  let systemMediaQuery = window.matchMedia('(prefers-color-scheme: dark)');
  function applyTheme(mode){
    localStorage.setItem(THEME_KEY, mode);
    if(mode === 'system'){
      document.documentElement.dataset.theme = systemMediaQuery.matches ? 'dark' : 'light';
    }else{
      document.documentElement.dataset.theme = mode;
    }
  }
  function initTheme(){
    const stored = localStorage.getItem(THEME_KEY) || 'system';
    applyTheme(stored);
    // select radio
    const input = document.querySelector(`input[name="theme"][value="${stored}"]`);
    if(input){ input.checked = true; }
  }
  initTheme();
  systemMediaQuery.addEventListener('change', () => {
    const stored = localStorage.getItem(THEME_KEY) || 'system';
    if(stored === 'system') applyTheme('system');
  });
  $$('.segmented input[name="theme"]').forEach(r => {
    r.addEventListener('change', () => applyTheme(r.value));
  });

  // ACCESSIBILITY TOGGLES
  const highContrast = $('#high-contrast');
  const reduceMotion = $('#reduce-motion');
  if(highContrast){
    highContrast.addEventListener('change', () => {
      document.body.classList.toggle('high-contrast', highContrast.checked);
    });
  }
  if(reduceMotion){
    reduceMotion.addEventListener('change', () => {
      document.body.classList.toggle('reduce-motion', reduceMotion.checked);
    });
  }

  // PAYMENTS
  const paymentList = $('#payment-list');
  const addPaymentBtn = $('#add-payment');
  const paymentModal = $('#payment-modal');
  const paymentForm = $('#payment-form');
  let lastActiveElement = null;

  function openModal(){
    lastActiveElement = document.activeElement;
    paymentModal.hidden = false;
    paymentModal.setAttribute('aria-hidden', 'false');
    trapFocus(paymentModal);
    setTimeout(() => $('#cardholder')?.focus(), 30);
  }
  function closeModal(){
    paymentModal.hidden = true;
    paymentModal.setAttribute('aria-hidden', 'true');
    releaseFocus();
    if(lastActiveElement) lastActiveElement.focus();
  }
  addPaymentBtn.addEventListener('click', openModal);
  paymentModal.addEventListener('click', (e) => {
    if(e.target.closest('[data-close-modal]')) closeModal();
  });
  document.addEventListener('keydown', (e) => {
    if(paymentModal.hidden) return;
    if(e.key === 'Escape') closeModal();
  });

  paymentForm.addEventListener('submit', (e) => {
    e.preventDefault();
    const holder = $('#cardholder').value.trim();
    const number = $('#cardnumber').value.replace(/\s+/g,'');
    const last4 = number.slice(-4) || '0000';
    const expiry = $('#expiry').value.trim() || '—';

    const li = document.createElement('li');
    li.className = 'payment-item';
    li.innerHTML = `
      <div class="method">
        <div class="card-logo" aria-hidden="true"></div>
        <div class="text">
          <div class="title">Card •••• ${last4}</div>
          <div class="sub muted">${holder ? holder + ' • ' : ''}Expires ${expiry}</div>
        </div>
      </div>
      <div class="default">
        <label class="radio">
          <input type="radio" name="default-payment" />
          <span>Default</span>
        </label>
      </div>
      <div class="actions">
        <button type="button" class="btn neutral small remove-payment">Remove</button>
      </div>
    `;
    paymentList.appendChild(li);

    closeModal();
    showToast('Payment method added');
    paymentForm.reset();
  });

  paymentList.addEventListener('click', (e) => {
    const btn = e.target.closest('.remove-payment');
    if(btn){
      const item = btn.closest('.payment-item');
      item?.remove();
      showToast('Payment method removed');
    }
  });

  // SAVE
  $('#save-btn').addEventListener('click', () => {
    showToast('Settings saved');
  });

  // TOAST
  let toastTimer = null;
  function showToast(text){
    const toast = $('#toast');
    toast.textContent = text;
    toast.hidden = false;
    clearTimeout(toastTimer);
    toastTimer = setTimeout(() => {
      toast.hidden = true;
    }, 2000);
  }

  // FOCUS TRAP for modal
  let trapCleanup = null;
  function trapFocus(container){
    const focusable = $$('button, [href], input, select, textarea, [tabindex]:not([tabindex="-1"])', container)
      .filter(el => !el.hasAttribute('disabled'));
    if(focusable.length === 0) return;
    const first = focusable[0];
    const last = focusable[focusable.length - 1];

    function onKey(e){
      if(e.key !== 'Tab') return;
      if(e.shiftKey){
        if(document.activeElement === first){
          e.preventDefault();
          last.focus();
        }
      }else{
        if(document.activeElement === last){
          e.preventDefault();
          first.focus();
        }
      }
    }
    document.addEventListener('keydown', onKey);
    trapCleanup = () => document.removeEventListener('keydown', onKey);
  }
  function releaseFocus(){
    if(trapCleanup){ trapCleanup(); trapCleanup = null; }
  }
})();
