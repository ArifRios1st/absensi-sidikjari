<?php

namespace App\Enums;

use App\Traits\EnumTrait;

enum SyncAbsensiStatusEnum : int
{
    use EnumTrait;

    case OK = 1;
    case FAILED = 2;
    case DELETED = 3;
}
