_devclean_complete() {
  local cur="${COMP_WORDS[COMP_CWORD]}"
  local commands="scan clean stats analyze"
  local options="--help --version --json --active-only --min-size --max-size --dry-run --sort --reverse"
  if [[ ${COMP_CWORD} -eq 1 ]]; then
    COMPREPLY=($(compgen -W "$commands" -- "$cur"))
  else
    COMPREPLY=($(compgen -W "$options" -- "$cur"))
  fi
}
complete -F _devclean_complete devclean
