(function(){
  const $ = (sel, root=document) => root.querySelector(sel);
  const $$ = (sel, root=document) => Array.from(root.querySelectorAll(sel));

  const tabPhone = $('#tab-phone');
  const tabEmail = $('#tab-email');
  const panelPhone = $('#form-phone');
  const panelEmail = $('#form-email');
  const panelOtp = $('#form-otp');

  function activateTab(tab){
    const isPhone = tab === tabPhone;
    tabPhone.classList.toggle('is-active', isPhone);
    tabEmail.classList.toggle('is-active', !isPhone);

    tabPhone.setAttribute('aria-selected', String(isPhone));
    tabEmail.setAttribute('aria-selected', String(!isPhone));

    panelPhone.classList.toggle('is-hidden', !isPhone);
    panelEmail.classList.toggle('is-hidden', isPhone);

    panelPhone.setAttribute('aria-hidden', String(!isPhone));
    panelEmail.setAttribute('aria-hidden', String(isPhone));

    (isPhone ? panelPhone : panelEmail).focus();
  }

  tabPhone.addEventListener('click', () => activateTab(tabPhone));
  tabEmail.addEventListener('click', () => activateTab(tabEmail));

  // Phone -> OTP flow
  const phoneForm = panelPhone;
  const phoneInput = $('#phone');
  const phoneError = $('#phone-error');
  const countrySelect = $('#country');
  phoneForm.addEventListener('submit', (e) => {
    e.preventDefault();
    const digitsOnly = phoneInput.value.replace(/\D/g, '');
    if(digitsOnly.length < 8){
      phoneError.hidden = false;
      phoneInput.setAttribute('aria-invalid', 'true');
      phoneInput.focus();
      return;
    }
    phoneError.hidden = true;
    phoneInput.removeAttribute('aria-invalid');

    // Simulate sending OTP
    switchToOtp();
  });

  function switchToOtp(){
    panelPhone.classList.add('is-hidden');
    panelEmail.classList.add('is-hidden');
    panelOtp.classList.remove('is-hidden');
    panelPhone.setAttribute('aria-hidden', 'true');
    panelEmail.setAttribute('aria-hidden', 'true');
    panelOtp.setAttribute('aria-hidden', 'false');
    focusFirstOtp();
  }

  // Email sign-in
  const emailForm = panelEmail;
  const emailInput = $('#email');
  const emailError = $('#email-error');
  const passwordInput = $('#password');
  const togglePasswordBtn = $('#toggle-password');

  emailForm.addEventListener('submit', (e) => {
    e.preventDefault();
    const emailOk = /^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(emailInput.value);
    if(!emailOk){
      emailError.hidden = false;
      emailInput.setAttribute('aria-invalid', 'true');
      emailInput.focus();
      return;
    }
    emailError.hidden = true;
    emailInput.removeAttribute('aria-invalid');

    if(passwordInput.value.length < 8){
      passwordInput.focus();
      return;
    }

    // Simulate sign in success
    alert('Signed in successfully (demo)');
  });

  togglePasswordBtn.addEventListener('click', () => {
    const isHidden = passwordInput.type === 'password';
    passwordInput.type = isHidden ? 'text' : 'password';
    togglePasswordBtn.textContent = isHidden ? 'Hide' : 'Show';
    togglePasswordBtn.setAttribute('aria-pressed', String(isHidden));
    passwordInput.focus();
  });

  // OTP behaviors
  const otpInputs = $$('.otp-input');
  const otpError = $('#otp-error');
  function focusFirstOtp(){
    otpInputs[0].focus();
  }
  otpInputs.forEach((input, idx) => {
    input.addEventListener('input', (e) => {
      const value = e.target.value.replace(/\D/g, '');
      e.target.value = value.slice(0,1);
      if(value && idx < otpInputs.length - 1){
        otpInputs[idx+1].focus();
      }
    });
    input.addEventListener('keydown', (e) => {
      if(e.key === 'Backspace' && !e.target.value && idx > 0){
        otpInputs[idx-1].focus();
      }
      if(e.key === 'ArrowLeft' && idx > 0){
        otpInputs[idx-1].focus();
        e.preventDefault();
      }
      if(e.key === 'ArrowRight' && idx < otpInputs.length - 1){
        otpInputs[idx+1].focus();
        e.preventDefault();
      }
    });
  });

  panelOtp.addEventListener('submit', (e) => {
    e.preventDefault();
    const code = otpInputs.map(i => i.value).join('');
    if(code.length !== 4){
      otpError.hidden = false;
      focusFirstOtp();
      return;
    }
    otpError.hidden = true;
    alert('Phone verified (demo)');
  });

  $('#resend-otp').addEventListener('click', () => {
    otpInputs.forEach(i => i.value = '');
    focusFirstOtp();
    // Simulate resend
    setTimeout(() => {
      // noop
    }, 500);
  });

  // SSO demo handlers
  ['apple-btn','google-btn','facebook-btn'].forEach(id => {
    const btn = document.getElementById(id);
    if(btn){
      btn.addEventListener('click', () => alert(`SSO via ${id.replace('-btn','')} (demo)`));
    }
  });
})();
