_devclean() {
    local cur prev words cword
    _init_completion || return

    case "${prev}" in
        --report)
            COMPREPLY=( $(compgen -W "markdown html csv json" -- "$cur") )
            return 0
            ;;
        --category)
            COMPREPLY=( $(compgen -W "python rust javascript java build package managers containers editors" -- "$cur") )
            return 0
            ;;
        --sort)
            COMPREPLY=( $(compgen -W "name size modified" -- "$cur") )
            return 0
            ;;
        --min-size|--max-size)
            COMPREPLY=( $(compgen -W "512KB 250MB 1GB 2GB 1TB" -- "$cur") )
            return 0
            ;;
    esac

    COMPREPLY=( $(compgen -W "scan analyze clean stats version --help -h --version -V --json --verbose --dry-run --force --active-only --category --exclude --sort --reverse --min-size --max-size --report" -- "$cur") )
}

complete -F _devclean devclean
