<?php

namespace App\Filament\App\Pages;

use App\Models\Device;
use Filament\Forms\Form;
use Filament\Forms\Components\TextInput;
use Filament\Forms\Concerns\InteractsWithForms;
use Filament\Forms\Contracts\HasForms;
use Filament\Pages\Page;
use Filament\Support\Exceptions\Halt;
use Filament\Actions\Action;
use Filament\Notifications\Notification;
use BezhanSalleh\FilamentShield\Traits\HasPageShield;

class EditDevice extends Page implements HasForms
{
    use InteractsWithForms;
    use HasPageShield;

    public ?array $data = [];

    protected ?string $heading = 'Device Settings';

    protected static ?string $title = 'Device Settings';

    protected static ?string $slug = 'device-settings';
    protected static ?string $navigationGroup = 'Settings';
    protected static ?string $navigationIcon = 'heroicon-o-cog-8-tooth';

    protected static string $view = 'filament.app.pages.edit-device';

    public function mount(): void
    {
        $this->form->fill(Device::firstOrCreate()->attributesToArray());
    }

    public function form(Form $form): Form
    {
        return $form
            ->schema([
                TextInput::make('token')
                    ->required(),
                TextInput::make('kapasitas')
                    ->required()
                    ->integer(),
            ])
            ->statePath('data');
    }

    protected function getFormActions(): array
    {
        return [
            Action::make('save')
                ->label(__('filament-panels::resources/pages/edit-record.form.actions.save.label'))
                ->submit('save'),
        ];
    }

    public function save(): void
    {
        try {
            $data = $this->form->getState();

            Device::firstOrCreate()->update($data);
        } catch (Halt $exception) {
            return;
        }

        Notification::make()
            ->success()
            ->title(__('filament-panels::resources/pages/edit-record.notifications.saved.title'))
            ->send();
    }

    protected function getShieldRedirectPath(): string {
        return '/';
    }
}
